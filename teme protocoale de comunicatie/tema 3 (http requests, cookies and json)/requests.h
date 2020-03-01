//Voicu Alex-Georgian (additions)

#ifndef _REQUESTS_
#define _REQUESTS_

char *compute_get_request(const char *host, const char *url, const char *url_params, const char *cookiesline, 
	const char * authorization);
char *compute_post_request(const char *host, const char *url, const char *form_data, const char *ctype, 
	const char *cookiesline, const char * authorization);

#endif
