<?php
/**
 * @file
 * Handling specific to a Ding1/D6 source for roles (user roles)
 */
class DingRoleMigration extends DrupalRole6Migration {
  public function __construct(array $arguments) {
    $role_arguments = array(
      'machine_name' => 'DingRole',
      'group_name' => 'ding1_group',
      'description' => t('Migration of Roles from Ding1/D6'),
      'source_connection' => 'legacy',
      'source_version' => 6,
      'role_mappings' => array(
        'administrator' => 'administrator',
        'author' => 'provider',
        'editor' => 'editor',
      ),
    );
    
    parent::__construct($role_arguments);

  }
}
