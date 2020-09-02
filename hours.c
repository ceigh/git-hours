#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <git2.h>

const char *get_default_email();

int main() {
  const char *default_email = get_default_email();

  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));

  git_repository *repo = NULL;
  git_revwalk *walker = NULL;

  git_libgit2_init();

  git_repository_open(&repo, cwd);
  git_revwalk_new(&walker, repo);
  git_revwalk_sorting(walker, GIT_SORT_REVERSE);
  git_revwalk_push_head(walker);

  git_oid oid;
  while(!git_revwalk_next(&oid, walker)) {
    git_commit *commit = NULL;
    git_commit_lookup(&commit, repo, &oid);
    const char *commit_email = (*git_commit_author(commit)).email;

    if (*commit_email != *default_email) continue;
    printf("%li %s\n", git_commit_time(commit), commit_email);

    git_commit_free(commit);
  }

  git_revwalk_free(walker);
  git_repository_free(repo);
  git_libgit2_shutdown();

  return 0;
}

const char *get_default_email() {
  const char *email = NULL;
  git_config *cfg = NULL;
  git_config *cfg_snapshot = NULL;

  git_libgit2_init();
  git_config_open_default(&cfg);
  git_config_snapshot(&cfg_snapshot, cfg);
  git_config_get_string(&email, cfg_snapshot, "user.email");

  git_config_free(cfg);
  git_libgit2_shutdown();
  return email;
}
