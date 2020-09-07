#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <git2.h>

#define VERSION "1.1.4"

int MAX_DIFF_MINUTES = 120;
int FIRST_COMMIT_MINUTES = 120;

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

char *cwd() {
  long size = pathconf(".", _PC_PATH_MAX);
  char *buf;

  if ((buf = (char *)malloc((size_t)size)) != NULL)
    return getcwd(buf, (size_t)size);
  else {
    perror("Error getting the path of the current working directory");
    exit(EXIT_FAILURE);
  }
}

void get_hours(
  long *hours, long *commits,
  const char *path_to_repo, const char *author_email
) {
  git_repository *repo = NULL;
  git_revwalk *walker = NULL;

  git_libgit2_init();
  git_repository_open(&repo, path_to_repo);
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
  float minutes_total = 0;
  long commits_total = 0;
  long prev_time = 0;

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

    const long time = author.when.time;
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
  if (minutes_total >= FIRST_COMMIT_MINUTES)
    minutes_total -= FIRST_COMMIT_MINUTES;

  /* free */
  git_revwalk_free(walker);
  git_repository_free(repo);
  git_libgit2_shutdown();

  /* point results */
  *hours = (long)(minutes_total / 60);
  *commits = commits_total;
}

void parse_opts(int argc, char **argv, char **email, char **path) {
  /* see
   * https://gnu.org/savannah-checkouts/gnu/libc/manual/html_node/Example-of-Getopt.html
   */
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
        if (system("man git hours")) exit(EXIT_SUCCESS);
        else {
          fprintf(stderr, "Command processor doesn't exists");
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

  /* get non-optional path arg */
  *path = argv[optind];
}

int main(int argc, char **argv) {
  char *email_opt_val = NULL, *path_opt_val = NULL;
  parse_opts(argc, argv, &email_opt_val, &path_opt_val);

  const char *email = email_opt_val == NULL
    ? get_default_email() : email_opt_val;
  const char *path = path_opt_val == NULL
    ? cwd() : path_opt_val;

  long commits = 0, hours = 0;
  get_hours(&hours, &commits, path, email);

  printf("%s\t%li\t%li\n", email, hours, commits);
  exit(EXIT_SUCCESS);
}
