/* The guardian that will watch over the files and will protect the user agains
 * evile ones */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <linux/limits.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include "sha-256.h"
#include "curl.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

/* Get the sha256 hash of a file
 * @file - the path to file
 * @return - jash as string, or NULL if fail */
char* quick_sha(const char* file)
{
	FILE * f = NULL;
	unsigned int i = 0;
	unsigned int j = 0;
	char buf[4096];
	uint8_t sha256sum[32];
	char *hash = malloc(1024);
	hash[0] = 0;

	if(!hash) {
		perror("hash - malloc");
		return NULL;
	}

	if( ! ( f = fopen( file, "rb" ) ) )
	{
		perror( "fopen" );
		free(hash);
		return NULL;
	}
	
	sha256_context ctx;
	sha256_starts( &ctx );

	while( ( i = fread( buf, 1, sizeof( buf ), f ) ) > 0 )
	{
		sha256_update( &ctx, (uint8_t *)buf, i );
	}

	sha256_finish( &ctx, sha256sum );

	for( j = 0; j < 32; j++ )
	{
		char h[16];
		snprintf(h, 16, "%02x", sha256sum[j] );
		strcat(hash, h);
	}

	// printf( "  %s\n", argv[1] );
	fclose(f);
	return hash;
}

static volatile int do_run = 1;
void sgn_handl(int sig)
{
  do_run = 0;
}

struct file_count {
  int count;
  char name[PATH_MAX];
};

/* A dummy vector, statci allocated, no need to warry about memory leacks ;) */
struct static_vec {
  int len;
  struct file_count files[256];
};

int static_vec_init(struct static_vec* v)
{
  v->len = 0;
  for(int i=0; i<256; ++i) {
    v->files[i].count = 0;
  }
  return 0;
}

/* Print how many times each file was accessed */
void print_files(struct static_vec *fls)
{
  for(int i=0; i<fls->len; ++i) {
    printf("file: %s, count: %d\n", fls->files[i].name, fls->files[i].count);
  }
}

/* Get the access times for a file */
int get_acces_no(struct static_vec *fls, const char *name)
{
  for(int i=0; i<fls->len; ++i) {
    if(strcmp(fls->files[i].name, name) == 0) {
      return fls->files[i].count;
    }
  }
  return -1;
}

/* Search the file in the vector, increment the counter if exist or create a
 * new entry */
int access_file(struct static_vec *fls, const char* name)
{
  for(int i=0; i<fls->len; ++i) {
    if(strcmp(fls->files[i].name, name) == 0) {
      fls->files[i].count++;
      return 0;
    }
  }
  /* Not countet until now -> new file */
  fls->len++;
  int n = fls->len;
  fls->files[n-1].count = 1;
  strncpy(fls->files[n-1].name, name, PATH_MAX-1);
  return 0;
}

/* Count the access times of a file in fls and print the event from inotify
 * @event - the inotify event to read
 * @fls - the simple static allocated vector with the file list */
void count_file(struct inotify_event *event, struct static_vec *fls)
{
  if ( event->mask & IN_CREATE ) {
    if ( event->mask & IN_ISDIR ) {
      printf( "New directory %s created.\n", event->name );
    }
    else {
      printf( "New file %s created.\n", event->name );
    }
  }
  else if (event->mask & IN_ACCESS ) {
    access_file(fls, event->name);
    int nr = get_acces_no(fls, event->name);
    printf( "File %s accessed %d times.\n", event->name, nr );
  }
}

/* Check if the path goes to a directory
 * @return - 1 if true, 0 if falsej */
int is_dir(const char *name)
{
  struct stat st;
  if(stat(name, &st) == -1) {
    perror("stat");
    exit(1);
  }
  if((st.st_mode & S_IFMT) == S_IFDIR) {
    return 1;
  }
  return 0;
}

/* Add a new watcher for the fd notifier
 * @fd - file descritpor for inotify
 * @wfds - static vector for watchers fds
 * @path - path to file
 * @reutrn - only 0 on succes */
int add_new_watcher(int fd, struct static_vec *wfds, const char *path)
{
	if(!path || !wfds) {
		return -1;
	}
	int wd = inotify_add_watch( fd, path,
			IN_ACCESS | IN_CREATE | IN_MODIFY | IN_DELETE);
	int n = wfds->len++;
	wfds->files[n].count = wd;
	strncpy(wfds->files[n].name, path, PATH_MAX);
	fprintf(stderr, "New wathcer for %s, wd: %d\n", path, wd);
	return 0;
}

/* Remove a watcher from inotify
 * @fd - file descritpor for inotify
 * @wfds - static vector for watchers fds
 * @wd - watcher to remove
 * @return - 0 on success, -1 on fail */
int remove_watcher(int fd, struct static_vec *wfds, int wd)
{
	for(int i=0; i<wfds->len; ++i){
		if(wfds->files[i].count == wd) {
			fprintf(stderr, "Removing watcher on: %s\n", wfds->files[i].name);
			int n = wfds->len;
			inotify_rm_watch(fd, wd );
			wfds->files[i].count = wfds->files[n-1].count;
			strncpy(wfds->files[i].name, wfds->files[n-1].name, PATH_MAX);
			wfds->len--;
			return 0;
		}
	}
	return -1;
}

/* Pause the watcher, it just removes the watchers but deos not alter the state
 * of the data structure
 * @fd - file descritpor for inotify
 * @wfds - static vector for watchers fds
 * @wd - watcher to suspend
 * @return - 0 on success, -1 on fail */
int pause_watcher(int fd, struct static_vec *wfds, int wd)
{
	for(int i=0; i<wfds->len; ++i){
		if(wfds->files[i].count == wd) {
			inotify_rm_watch(fd, wd );
			return 0;
		}
	}
	return -1;
}

/* Resume a suspended watcher, just create other wathcer on the place the old one
 * was stored. Does not alter the state of the data structure
 * @fd - file descritpor for inotify
 * @wfds - static vector for watchers fds
 * @wd - watcher to resume
 * @return - new wd on success, -1 on fail */
int resume_watcher(int fd, struct static_vec *wfds, int wd)
{
	for(int i=0; i<wfds->len; ++i){
		if(wfds->files[i].count == wd) {
			int wd = inotify_add_watch( fd, wfds->files[i].name, IN_ACCESS | IN_CREATE | IN_MODIFY);
			wfds->files[i].count = wd;
			return wd;
		}
	}
	return -1;
}

char *get_wd_path(struct static_vec *wfds, int wd)
{
  for(int i=0; i<wfds->len; ++i) {
    if(wfds->files[i].count == wd) {
      return wfds->files[i].name;
    }
  }
  return NULL;
}

int rm_watchers(int fd, struct static_vec *wfds)
{
	for(int i=0; i<wfds->len; ++i) {
		inotify_rm_watch( fd, wfds->files[i].count);
	}
	return 0;
}

int main(int argc, char** argv)
{
  int length, i = 0;
  int fd;
  char buffer[EVENT_BUF_LEN];
  struct static_vec fls;
  struct static_vec wfds;	// watchers
  static_vec_init(&fls);
  static_vec_init(&wfds);
  signal(SIGINT, sgn_handl);
  if (argc < 2) {
    fprintf(stderr, "Please provide a directory to watch on\n"
        "Usage: %s dir_name\n", argv[0]);
    exit(1);
  }

  /*creating the INOTIFY instance*/
  fd = inotify_init();

  /*checking for error*/
  if ( fd < 0 ) {
    perror( "inotify_init" );
  }

  /*adding the “/tmp” directory into watch list. Here, the suggestion is to
   * validate the existence of the directory before adding into monitoring
   * list.*/
  if(0 == is_dir(argv[1])) {
    fprintf(stderr, "No such directory: %s\n", argv[1]);
    exit(1);
  }

  add_new_watcher(fd, &wfds, argv[1]);

  /*read to determine the event change happens on dir_name directory. Actually
   * this read blocks until the change event occurs*/ 
  do{
    i = 0;
    length = read( fd, buffer, EVENT_BUF_LEN ); 

    /*checking for error*/
    if ( length < 0 ) {
      perror( "read" );
    }  

    /*actually read return the list of change events happens. Here, read the
     * change event one by one and process it accordingly.*/
    while ( i < length ) {     
      struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
      if ( event->len ) {
			char *dir_path = get_wd_path(&wfds, event->wd);
			char f_path[PATH_MAX];
			snprintf(f_path, PATH_MAX-1, "%s/%s", dir_path, event->name);
			printf("main total path: %s\n", f_path);
			// remove the watcher to don't count our own file read
			// inotify_rm_watch(fd, event->wd);
			pause_watcher(fd, &wfds, event->wd);
			count_file(event, &fls);
			char *hash = quick_sha(f_path);
			if(((event->mask & IN_MODIFY) || (event->mask & IN_CREATE))
				   	&& hash) {
				printf("%s hash: %s\n", event->name, hash);
				if(virus_total_api(hash)) {
					printf("VIRUSE DETECTED: %s\n", f_path);
					remove(f_path);
				}
				free(hash);
			}
			// put back the watcher
			resume_watcher(fd, &wfds, event->wd);
			if ( (event->mask & IN_ISDIR) ) {
				if (event->mask & IN_CREATE)
					add_new_watcher(fd, &wfds, f_path);
				if (event->mask & IN_DELETE) {
					fprintf(stderr, "Got to remove wd: %d\n", event->wd);
					remove_watcher(fd, &wfds, event->wd);
				}

			}
      }
      i += EVENT_SIZE + event->len;
    }
  }while(do_run);

  /*closing the INOTIFY instance*/
  close( fd );

  printf("The files:\n");
  print_files(&fls);
}
