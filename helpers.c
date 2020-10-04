#include <stdio.h>
#include <unistd.h>

#include "helpers.h"

/* get user email from .gitconfig */
const char *get_default_email(void) {
  const char *email = NULL;
  git_config *cfg = NULL;
  git_config *cfg_snapshot = NULL;

  git_libgit2_init();
  git_config_open_default(&cfg);
  git_config_snapshot(&cfg_snapshot, cfg);
  git_config_get_string(&email, cfg_snapshot, "user.email");

  git_config_free(cfg);
  git_libgit2_shutdown();

  if (email == NULL) {
    fprintf(stderr, "Empty git user email!\n");
    exit(EXIT_FAILURE);
  }
  return email;
}

const char *cwd(void) {
  long size = pathconf(".", _PC_PATH_MAX);
  char *buf;

  if ((buf = (char *)malloc((size_t)size)) != NULL)
    return getcwd(buf, (size_t)size);
  else {
    perror("Error getting the path of the current working directory");
    exit(EXIT_FAILURE);
  }
}
