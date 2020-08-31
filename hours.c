#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <git2.h>

int main() {
  char cwd[PATH_MAX];
  git_repository *repo = NULL;
  git_revwalk *walker = NULL;

  getcwd(cwd, sizeof(cwd));
  printf("cwd: %s\n\n", cwd);

  git_libgit2_init();
  git_repository_open(&repo, cwd);
  git_revwalk_new(&walker, repo);
  git_revwalk_sorting(walker, GIT_SORT_REVERSE);
  git_revwalk_push_head(walker);

  git_oid oid;
  while(!git_revwalk_next(&oid, walker)) {
    git_commit *commit = NULL;
    git_commit_lookup(&commit, repo, &oid);

    printf("%li\n", git_commit_time(commit));

    git_commit_free(commit);
  }

  git_revwalk_free(walker);
  git_repository_free(repo);
  git_libgit2_shutdown();

  return 0;
}
