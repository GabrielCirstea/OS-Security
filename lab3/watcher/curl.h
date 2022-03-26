/* Do the API call with libcurl and check if the response indicate a virus */
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#define MULT 10 * 4096

struct content_data {
	int size;
	char *data;
};

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	struct content_data *cont = userdata;
	int realsize = size * nmemb;
	char *tmp = realloc(cont->data, cont->size + realsize + 1);
	if(tmp == NULL)
		return 0;  /* out of memory! */
	cont->data = tmp;
	memcpy(&(cont->data[cont->size]), ptr, realsize);
	cont->size += realsize;
	cont->data[cont->size - 1] = 0;

	return realsize;
}

int check_virus(const char *data)
{
	if(strstr(data, "\"malicious\",\n") != NULL) {
		return 1;
	}
	if(strstr(data, "\"harmless\",\n") != NULL) {
		return 1;
	}
	return 0;
}
 
/* Call the damn API and cehck if we have a evil file in hands
 * @return 1 if evil file, 0 if not */
int virus_total_api(char* hash)
{
	CURL *curl;
	CURLcode res;
	struct content_data content;
	char url[1024];
	char api_url[] = "https://www.virustotal.com/api/v3/search";
	char virus_detected = 0;

	if(!hash) {
		return 1;
	}
	snprintf(url, 1023, "%s?query=%s", api_url, hash);
	// printf("curl url = %s\n", url);

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(curl, CURLOPT_URL, url);

		content.data = NULL;
		content.size = 0;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

		// for API v3, directly from virustotal API docs
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "x-apikey: "
				"03d23fe2e7dd6ec125e7f534318341c8ad07a18b39f59b4caf4d10e523e3856e");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
		curl_easy_strerror(res));

		// ugly solution for '\0' in the middle of the string
		for(int i=0; i<content.size-1; ++i) {
			if(content.data[i] == 0)
				content.data[i] = '.';
		}
		virus_detected = check_virus(content.data);

		/* always cleanup */ 
		curl_easy_cleanup(curl);
		free(content.data);
	} else {
		perror("curl init");
	}
	return (int)virus_detected;
}
