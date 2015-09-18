## Drush `list-unused` command

```list-unused``` does its best at listing directories which you don't need anymore - i.e. folders of those modules and themes which are either disabled or had never been installed.

**Warning: while this software is itself safe, it's at early development, so please use it with caution, double check before trying any real file operations.**

## Usage

**List directories**
```
drush list-unused
```
sample output:
```
...
sites/all/modules/git_deploy
sites/all/modules/gmap
sites/all/modules/gridbuilder
sites/all/modules/hacked
sites/all/modules/hierarchical_select
sites/all/modules/i18n
sites/all/modules/imce
...
```
**Delete directories**
In theory it can be done like so:
```
drush list-unused | while IFS= read -r line; do rm -rf "$line"; done
```
But I myself haven't tried it and can't think why you may need this. Instead use the output to feed your deployment system. For example I use it to create exclude list for ```rsync``` when deploying a website.

## TODO

Add dependency search for:

* base themes
* libraries

## Alternatives

There is [Drush Extras] (https://www.drupal.org/project/drush_extras) project on Drupal.org. It provides a set of handy drush commands including ```pm-projectinfo``` and recently was patched so that now it can list paths. Example usage:
```
drush pm-projectinfo --status=disabled --fields=path --format=list
```
**Note however one important caveat:** it doesn't support git-fetched projects (modules) so it will simply skip all of them. You should use [Git deploy] (https://www.drupal.org/project/git_deploy) module to get things done in this case.

## Motivation

When you finish developing a website, you may end up with tens of disabled/never installed modules under ```sites/*/modules``` which you probably don't want to keep for production usage. It would be nice to have a ```drush``` command to just delete those modules or at least to enumerate their directories. Unfortunately out of the box ```drush``` can only help you in figuring out the disabled/not installed module *names*, not their location:
```
drush pm-list --no-core --pipe --type=module --status='not installed'
```
and the sample output:
```
...
i18n_block
i18n_contact
i18n_field
i18n
i18n_menu
i18n_node
i18n_forum
...
```
— i.e. no paths. Also, it lists submodules just the same way it lists modules.

## Approach

Had we 1-to-1 relation between directories and projects, the task wouldn't ever worth to mention. But Drupal architecture allows projects to have several modules in it, share one directory among different modules, have random nesting and even modules location. In such circumstances there is no probably 100%-working solution for the task of fnding unused locations (paths). That said, we have to make at least this one assumption about projects locations to get things done:

* submodules which are the part of their projects were not moved outside the containing project directory

Hopefully, in real life such a heresy is rare at least.

Now when the stage has been set, the idea is pretty simple: get the list of disabled modules' paths from the database and filter it out by two criteria:

1. a path should not be a part of *some enabled* module's path
2. a path should not be a part of *parent disabled* module's path

1st rule basically means that we ignore disabled submodules if their parents are enabled. This is needed to not break the integrity of projects containing several submodules. Example: ```i18n``` with lots of submodules. Had we counted a disabled submodule (like ```i18n_block```) as unused, we would break ```i18n``` project integrity.

2st rule is needed to fold up disabled submodules' dirs to their parents. I.e. all those ```i18n_block```, ```i18n_contact```, ```i18n_field``` and etc should fold up to their parent - ```i18n```.

## Testing

Please report any troubles and share ideas on enhancing/fixing this utility.
