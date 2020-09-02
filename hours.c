#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <git2.h>

#define MAX_DIFF_MINUTES 120
#define FIRST_COMMIT_MINUTES 120

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

int main() {
  const char *default_email = get_default_email();

  /* get current working directory path */
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));

  /* init libgit */
  git_repository *repo = NULL;
  git_revwalk *walker = NULL;

  git_libgit2_init();
  git_repository_open(&repo, cwd);

  /* set up history walker */
  git_revwalk_new(&walker, repo);
  /* commits from old to new */
  git_revwalk_sorting(walker, GIT_SORT_REVERSE);
  git_revwalk_push_head(walker);

  /* set counters */
  git_oid oid;
  int commits_total = 0;
  float minutes_total = 0;
  long int prev_time = 0;

  while(!git_revwalk_next(&oid, walker)) {
    /* get commit data */
    git_commit *commit = NULL;
    git_commit_lookup(&commit, repo, &oid);
    const git_signature *author = git_commit_author(commit);

    /* filter commits - only from one author */
    const char *commit_email = (*author).email;
    if (*commit_email != *default_email) {
      git_commit_free(commit);
      continue;
    }

    /* now we can start to count commits and time */
    commits_total++;

    const long int time = (*author).when.time;
    /* difference between commits in minutes */
    const float diff = (time - prev_time) / 60.0;
    /* replace previous commit time with current */
    prev_time = time;

    /* skip first commit difference */
    if (commits_total == 1) {
      git_commit_free(commit);
      continue;
    }

    /* if difference <= our group limit - add it,
     * otherwise this is first commit in group,
     * and just add default value
     */
    if (diff <= MAX_DIFF_MINUTES) minutes_total += diff;
    else minutes_total += FIRST_COMMIT_MINUTES;

    git_commit_free(commit);
  }
  printf("%d\n", (int)minutes_total / 60);

  git_revwalk_free(walker);
  git_repository_free(repo);
  git_libgit2_shutdown();

  return 0;
}
