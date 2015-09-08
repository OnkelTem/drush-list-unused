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

  // Get the list of all modules under sites/**
  $result = drush_db_select('system', array('filename', 'status'), 'filename like :fname', array(':fname' => 'sites%') , NULL, NULL, 'filename', 'ASC');

  $modules = array();
  $statuses = array();
  $i = 0;
  foreach($result as $record) {
    $modules[$i] = dirname($record->filename);
    $statuses[$i] = $record->status;
    $i++;
  }

  $unused = array();
  foreach($modules as $key => $path) {
    // Skip enabled modules
    if ($statuses[$key]) {
      continue;
    }
    $_path = $path;
    $submodule = FALE;
    // Search for a parent project up to the 'sites' directory in $modules array - i.e. both enabled and disabled modules
    // as to count a module 'unused' it should be a full project, not a submodule.
    while(($_path = dirname($_path)) !== 'sites') {
      if (in_array($_path, $modules)) {
        $submodule = TRUE;
        continue;
      }
    }
    if (!$submodule) {
      $unused[] = $path;
    }
  }
  $unused = array_unique($unused, SORT_STRING);
  array_walk($unused, function($el) { print $el . "\n"; });
}
