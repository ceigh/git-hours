# 🕰️ hours
> Tool for count the time spent on code via git.

## Why

Previously, I used [git-hours](https://github.com/kimmobrunfeldt/git-hours) tool, but due to the impossibility of a normal installation, and questionable work speed, it was decided to create my own utility.

It is very easy to use.

## Usage

### TL;DR

`hours[ -e <email>][ <path>]`

### Details

Just call `hours` in repository directory. That's it. It automatically use your email from default `.gitconfig` file, and count hours. Now you can see result:

```shell
ceigh@arch ~/Work/hours ⎇ master $ hours
ceigh@pm.me    16    19
```

Three columns: author email, total hours spended and total commits from this author.

If you don't have `.gitconfig` file, you can create it, see `GIT-CONFIG(1)`.

Also you can pass `-e` flag to command, with desired email address:

```shell
ceigh@arch ~/Work/hours ⎇ master $ hours -e ceigh@pm.me
ceigh@pm.me    16    19
```

And of course you can specify path to repository as non optional argument:

```shell
ceigh@arch ~ $ hours Work/hours
ceigh@pm.me    16    19
```

For more information check `man hours`.

## Algorithm

For detail explanation, you can see original git-hours [HIW](https://github.com/kimmobrunfeldt/git-hours#how-it-works). I used very similar method.

## Installation

As a main dependency i use [libgit2](https://github.com/libgit2/libgit2), to you need to install it in your system first.

After it, you can just

```shell
git clone git@github.com:ceigh/hours.git
cd hours
make
sudo make install
```

## Contribution

If you find a bug or something, please open an [issue](https://github.com/ceigh/hours/issues/new).

## License

GNU General Public License v3.0.