#include <curl/curl.h>

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t n = fwrite(ptr, size, nmemb, (FILE *) userdata);
	fprintf(stderr, "curl fwrite: n = %ld\n", n);
	return n;
}

 
int virus_total_api(char* hash)
{
	CURL *curl;
	CURLcode res;
	FILE * f = NULL; 
	char url[1024];
	char api_url[] = "https://www.virustotal.com/api/v3/search";

	if(!hash) {
		return 1;
	}
	snprintf(url, 1023, "%s?query=%s", api_url, hash);
	printf("curl url = %s\n", url);

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(curl, CURLOPT_URL, url);

		f = fopen("tmp_hash.txt", "w+");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

		// for API v3, directly from virustotal API docs
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "x-apikey: "
				"<key>");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
		curl_easy_strerror(res));

		/* always cleanup */ 
		curl_easy_cleanup(curl);
		fclose(f);
	} else {
		perror("curl init");
	}
	return 0;
}
