#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "helpers.h"

unsigned short MAX_DIFF_MINUTES = 120;
unsigned short FIRST_COMMIT_MINUTES = 120;

/* to get result of diff_file_cb() */
bool _is_file_in_diff = false; // TODO: how to make not global?
/* https://libgit2.org/libgit2/#HEAD/group/callback/git_diff_file_cb
 * can't return or point useful value, so I use global variable here
 */
int diff_file_cb(const git_diff_delta *delta, float progress, void *file_name) {
  /* if file_name in diff */
  if (!strcmp(file_name, delta -> old_file.path) ||
    !strcmp(file_name, delta -> new_file.path)) _is_file_in_diff = true;
  return 0;
}
const int check_that_file_in_diff(
  git_repository *repo, /* need to git_diff_tree_to_tree() */
  const git_commit *commit,
  char *file_name,
  bool *out_is_file_in_diff
) {
  int return_code = 0;

  unsigned parentcount = git_commit_parentcount(commit);
  /* to track changes from all parents */
  for (unsigned i = 0; i < parentcount; ++i) {
    git_commit *parent_commit = NULL;
    git_commit_parent(&parent_commit, commit, i);

    /* commit trees to get diff */
    git_tree *parent_tree = NULL;
    git_tree *tree = NULL;
    git_commit_tree(&parent_tree, parent_commit);
    git_commit_tree(&tree, commit);

    /* https://libgit2.org/libgit2/#HEAD/group/diff/git_diff_tree_to_tree */
    git_diff *diff = NULL;
    git_diff_tree_to_tree(&diff, repo, parent_tree, tree, NULL);

    _is_file_in_diff = false;
    return_code = git_diff_foreach(diff, diff_file_cb, NULL, NULL, NULL, file_name);
    *out_is_file_in_diff = _is_file_in_diff;

    /* free */
    git_diff_free(diff);
    git_tree_free(tree);
    git_tree_free(parent_tree);
    git_commit_free(parent_commit);
  }

  return return_code;
}

void get_hours(
  unsigned long *hours,
  unsigned long *commits,
  const char *author_email,
  char *file_name
) {
  git_repository *repo = NULL;
  git_revwalk *walker = NULL;

  git_libgit2_init();
  git_repository_open(&repo, cwd());
  if (repo == NULL) {
    perror("Error opening repository");
    exit(EXIT_FAILURE);
  }

  /* set up history walker */
  git_revwalk_new(&walker, repo);
  /* commits from old to new */
  git_revwalk_sorting(walker, GIT_SORT_REVERSE);
  git_revwalk_push_head(walker);

  git_oid oid;
  float minutes_total = 0.0f;
  unsigned long commits_total = 0;
  unsigned prev_time = 0;

  while(!git_revwalk_next(&oid, walker)) {
    git_commit *commit = NULL;
    git_commit_lookup(&commit, repo, &oid);

    /* filter commits - only from one author */
    const git_signature author = *git_commit_author(commit);
    if (strcmp(author_email, author.email)) {
      git_commit_free(commit);
      continue;
    }

    /* filter commits - only with specified file */
    if (file_name != NULL) { /* if file name provided */
      bool is_file_in_diff = false;
      check_that_file_in_diff(repo, commit, file_name, &is_file_in_diff);
      if (!is_file_in_diff) {
        git_commit_free(commit);
        continue;
      }
    }

    /* now we can start to count commits and time */
    commits_total++;

    const unsigned time = author.when.time;
    /* difference between commits in minutes */
    const float diff = (time - prev_time) / 60.0f;
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
  if (minutes_total >= FIRST_COMMIT_MINUTES)
    minutes_total -= FIRST_COMMIT_MINUTES;

  /* free */
  git_revwalk_free(walker);
  git_repository_free(repo);
  git_libgit2_shutdown();

  /* point results */
  *hours = (unsigned long)(minutes_total / 60.0f);
  *commits = commits_total;
}

/* https://gnu.org/savannah-checkouts/gnu/libc/manual/html_node/Example-of-Getopt.html */
void parse_opts(const int argc, char **argv, char **email, char **file_name) {
  int c;
  opterr = 0;

  while ((c = getopt(argc, argv, "d:e:f:hv")) != -1) {
    switch (c) {
      case 'd':
        MAX_DIFF_MINUTES = atoi(optarg);
        if (!MAX_DIFF_MINUTES) {
          fprintf(stderr, "Option -%c requires number > 0\n", c);
          exit(EXIT_FAILURE);
        } else break;

      case 'e':
        *email = optarg;
        break;

      case 'f':
        FIRST_COMMIT_MINUTES = atoi(optarg);
        if (!FIRST_COMMIT_MINUTES) {
          fprintf(stderr, "Option -%c requires number > 0\n", c);
          exit(EXIT_FAILURE);
        } else break;

      case 'h':
        if (!(system("man git hours"))) exit(EXIT_SUCCESS);
        else {
          fprintf(stderr, "Command processor doesn't exists\n");
          exit(EXIT_FAILURE);
        }

      case 'v':
        printf("%s\n", VERSION);
        exit(EXIT_SUCCESS);

      case '?':
        if (optopt == 'd' || optopt == 'e' || optopt == 'f')
          fprintf(stderr, "Option -%c requires an argument\n", optopt);
        else if (isprint(optopt))
          fprintf (stderr, "Unknown option `-%c`\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x`\n", optopt);
        exit(EXIT_FAILURE);

      default:
        abort();
    }
  }

  /* get non-optional file_name arg */
  *file_name = argv[optind];
}

int main(const int argc, char **argv) {
  char *email_opt_val = NULL;
  char *file_name = NULL;
  parse_opts(argc, argv, &email_opt_val, &file_name);

  const char *email = email_opt_val == NULL
    ? get_default_email() : email_opt_val;

  unsigned long commits = 0, hours = 0;
  get_hours(&hours, &commits, email, file_name);

  printf("%s\t%li\t%li\n", email, hours, commits);
  exit(EXIT_SUCCESS);
}
