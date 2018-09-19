/*****************************************************
 * Auth.c
 *
 * Authentication interface. Serves as an abstraction
 * layer to various authentication methods.
 *****************************************************/
extern int authenticate(char *user, char *pass);

int authenticate(char *user, char *pass) {
#ifdef USE_SHADOW_AUTH
	return shadow_auth(user, pass);
#else /* No other currently supported authentication methods. */
	return 0;
#endif
}
