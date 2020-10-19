# git-hours
> Tool for count the time spent on code via git.

[![AUR version](https://img.shields.io/aur/version/git-hours)](https://aur.archlinux.org/packages/git-hours)
[![AUR last modified](https://img.shields.io/aur/last-modified/git-hours)](https://aur.archlinux.org/packages/git-hours)

## Why

Previously, I used [git-hours](https://github.com/kimmobrunfeldt/git-hours) tool, but due to the impossibility of a normal installation, and questionable work speed, it was decided to create my own utility.

Easy to use.

## Usage

### TL;DR

`git hours [-e email] [-d max_commits_difference_minutes] [-f first_commit_minutes] [file_name]`

### Details

Just call `git hours` in repository directory. That's it. It automatically use your email from default `.gitconfig` file, and count hours. Now you can see result:

```shell
git hours
me@org.com    16    19
```

Three columns: author email, total hours spended and total commits from this author.

If you don't have `.gitconfig` file, you can create it, see `git-config(1)`.

Also you can pass `-e` flag to command, with desired email address:

```shell
git hours -e me@org.com
me@org.com    16    19
```

To specify path to repository, you can use git's `-C` flag:

```shell
git -C /path/to/repo/ hours
me@org.com    16    19
```

Note that in first we pass flags for `git`, and only then for `hours`.

If you want to restrict calculations only for one specific file in repository:

```shell
git hours file_name
me@org.com    6    9
```

But it may be _slow_ on repos with big amount of commits.

For more information check `man git hours`.

## Algorithm

For detail explanation, you can see original git-hours [HIW](https://github.com/kimmobrunfeldt/git-hours#how-it-works). I used similar method.

## Installation

### Package manager

For now, you can install it via [AUR](https://aur.archlinux.org/packages/git-hours):

```
yay -S git-hours
```

### Manual

As a main dependency i use [libgit2](https://github.com/libgit2/libgit2), so you need to install it in your system first.

After it, you can just

```shell
git clone git@github.com:ceigh/git-hours.git
cd git-hours
make
sudo make install
```

*To delete this program, call `sudo make uninstall`.*

## Configuration

To set your own minimal commits interval, use `-d` key:

```shell
git hours -d 120 # stack commits only if between them <= 2 hours
```

> `-d` value must be number > 0.

Also you can change first commit balance time with `-f` key:

```shell
git hours -f 120 # add 2 hours for every first commit in stack
```

> `-f` value must be number > 0.

By default both this values are `120`.

## Contribution

If you find a bug or something, please open an [issue](https://github.com/ceigh/git-hours/issues/new).

## License

GNU General Public License v3.0.
