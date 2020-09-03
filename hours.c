#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <git2.h>

#define MAX_DIFF_MINUTES 120
#define FIRST_COMMIT_MINUTES 120

/* get user email from .gitconfig */
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

  if (email == NULL) {
    fprintf(stderr, "Empty git user email!\n");
    exit(EXIT_FAILURE);
  }
  return email;
}

int get_hours(const char *path_to_repo, const char *author_email, const int **commits) {
  git_repository *repo = NULL;
  git_revwalk *walker = NULL;

  git_libgit2_init();
  git_repository_open(&repo, path_to_repo);

  /* set up history walker */
  git_revwalk_new(&walker, repo);
  /* commits from old to new */
  git_revwalk_sorting(walker, GIT_SORT_REVERSE);
  git_revwalk_push_head(walker);

  git_oid oid;
  int commits_total = 0;
  float minutes_total = 0;
  long int prev_time = 0;

  while(!git_revwalk_next(&oid, walker)) {
    git_commit *commit = NULL;
    git_commit_lookup(&commit, repo, &oid);
    const git_signature author = *git_commit_author(commit);

    /* filter commits - only from one author */
    if (strcmp(author_email, author.email)) {
      git_commit_free(commit);
      continue;
    }

    /* now we can start to count commits and time */
    commits_total++;

    const long int time = author.when.time;
    /* difference between commits in minutes */
    const float diff = (time - prev_time) / 60.0;
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

  /* to balance last commit minutes */
  if (minutes_total >= FIRST_COMMIT_MINUTES) minutes_total -= FIRST_COMMIT_MINUTES;

  /* free */
  git_revwalk_free(walker);
  git_repository_free(repo);
  git_libgit2_shutdown();

  *commits = &commits_total;
  return (int)(minutes_total / 60);
}

int main() {
  const char *default_email = get_default_email();

  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));

  const int *commits = NULL;
  const int hours = get_hours(cwd, default_email, &commits);

  printf("%s\t%d\t%d\n", default_email, hours, *commits);
  exit(EXIT_SUCCESS);
}
