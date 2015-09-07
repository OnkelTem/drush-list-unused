<?php
/**
 * @file
 * Description
 */

/**
 * Implements hook_drush_help().
 */
function list_unused_drush_help($section) {
  switch ($section) {
    case 'drush:list-unused':
      return dt('List unused modules directories.');
  }
}

/**
 * Implements hook_drush_command().
 */
function list_unused_drush_command() {
  $items = array();

  $items['list-unused'] = array(
    'description' => 'List unused modules directories.',
  );

  return $items;
}

/**
 * Rebuild the registry.
 *
 * Before calling this we need to be bootstrapped to DRUPAL_BOOTSTRAP_DATABASE.
 */
function drush_list_unused() {
  // Collect enabled modules in 'sites/**'
  $result = drush_db_select('system', 'filename', 'status = :st', array(':st' => 1), NULL, NULL, 'filename', 'DESC');
  $enabled_paths = array();
  foreach($result as $record) {
    $path = dirname($record->filename);
    $enabled_paths[] = $path;
  }
  $enabled_paths = array_unique($enabled_paths);

  // Build unused paths list (sort is important)
  $result = drush_db_select('system', 'filename', 'status = :st and filename like :fname', array(':st' => 0, ':fname' => 'sites%') , NULL, NULL, 'filename', 'ASC');
  $unused_paths = array();
  foreach($result as $record) {
    $path = dirname($record->filename);
    // Skip subpaths of enabled paths
    if (in_array(dirname($path), $enabled_paths)) {
      continue;
    }
    // Skip subpaths of unused paths 
    if (in_array(dirname($path), $unused_paths)) {
      continue;
    }
    $unused_paths[] = $path;
  }
  $unused_paths = array_unique($unused_paths, SORT_STRING);
  array_walk($unused_paths, function($el) { print $el . "\n"; });
}

