<?php

/**
 * Common mappings for the Drupal 6 node migrations.
 */
abstract class DingNodeMigration extends DrupalNode6Migration {
  public function __construct(array $arguments) {
    $common_arguments = array(
      'source_connection' => 'legacy',
      'source_version' => 6,
      'user_migration' => 'User',
    );
    
    parent::__construct($arguments + $common_arguments);

    // With migrate_ui enabled, migration pages will indicate people involved in
    // the particular migration, with their role and contact info. We default the
    // list in the shared class; it can be overridden for specific migrations.
    $this->team = array(
      new MigrateTeamMember('Ewan Andreasen', 'ea@vejlebib.dk', t('Webdeveloper (Ding1 -> Ding2 Migration)')),
    );

    $this->addFieldMapping('field_legacy_nid', 'nid')
         ->description('We have a common field to save the D6 nid');
         
    // Unmapped source fields - EXAMPLE
    //$this->addUnmigratedSources(array('vid', 'tnid', 'translate', 'teaser', 'format'));
    // Unmapped destination fields - EXAMPLE
    //$this->addUnmigratedDestinations(array('log', 'tnid', 'translate', 'body:summary', 'body:format', 'body:language', 'comment'))
  }
}


class DingLibraryMigration extends DingNodeMigration {
  public function __construct() {
    
    $arguments = array(
      'description' => t('Migration of Libraries from Ding1/D6'),
      'machine_name' => 'DingLibrary',
      'source_type' => 'library',
      'destination_type' => 'ding_library',
      //'dependencies' => array('Tags'), //figure this out with dependencies
    );

    parent::__construct($arguments);

    $this->addFieldMapping('field_ding_library_body', 'body');
    $this->addFieldMapping('field_ding_library_body:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_library_body:format')
         ->defaultValue('ding_wysiwyg');
    $this->addFieldMapping('field_ding_library_lead', 'teaser');
    $this->addFieldMapping('field_ding_library_lead:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_library_mail', 'field_email');
    $this->addFieldMapping('group_group')
         ->defaultValue('1');

    // Phone and fax comes from location_phone table and is drawn in via the modified query
    $this->addFieldMapping('field_ding_library_phone_number', 'phone'); 
    $this->addFieldMapping('field_ding_library_phone_number:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_ding_library_fax_number', 'fax'); 
    $this->addFieldMapping('field_ding_library_fax_number:language')
         ->defaultValue('und');

    // library slug comes from ding_library table and is drawn in via the modified query
    $this->addFieldMapping('field_ding_library_slug', 'slug'); 
    $this->addFieldMapping('field_ding_library_slug:language')
         ->defaultValue('und');

    // Map to addressfield. See addressfield.inc in migrate_extras for available argument keys.
    $address_arguments = array(
      'field_ding_library_addresse_thoroughfare' => array('source_field' => 'street'),
      'field_ding_library_addresse_premise' => array('source_field' => 'additional'),
      'field_ding_library_addresse_locality' => array('source_field' => 'city'),
      'field_ding_library_addresse_sub_administrative_area' => array('source_field' => 'province'),
      'field_ding_library_addresse_postal_code' => array('source_field' => 'postal_code'),
      'field_ding_library_addresse_first_name' => array('source_field' => 'field_name_first'),
      'field_ding_library_addresse_last_name' => array('source_field' => 'field_name_last'),
    );
    // Note that of the country field is NULL, none of the values will be migrated!
    $this->addFieldMapping('field_ding_library_addresse', 'country')
         ->arguments($address_arguments);
 
    // Since the excerpt is mapped via an argument, add a null mapping so it's not flagged as unmapped.
    $this->addFieldMapping(NULL, 'street');
    $this->addFieldMapping(NULL, 'additional');
    $this->addFieldMapping(NULL, 'city');
    $this->addFieldMapping(NULL, 'province');
    $this->addFieldMapping(NULL, 'postal_code');
    $this->addFieldMapping(NULL, 'country');
    $this->addFieldMapping(NULL, 'latitude');
    $this->addFieldMapping(NULL, 'longitude');
    $this->addFieldMapping(NULL, 'lid');
    
    // Map to geofield. See geofield.inc in migrate_extras for available argument keys.
    $geo_arguments = array(
      'lat' => array('source_field' => 'latitude'),
      'lon' => array('source_field' => 'longitude'),
    );
    $this->addFieldMapping('field_ding_library_geocode')
         ->defaultValue('Point')
         ->arguments($geo_arguments);
    // Since the excerpt is mapped via an argument, add a null mapping so it's not flagged as unmapped.
    $this->addFieldMapping(NULL, 'latitude');
    $this->addFieldMapping(NULL, 'longitude');
         
    /*
    $this->addFieldMapping('field_image', 'field_lead_graphic')
         ->sourceMigration('Image');
    // Note that we map migrated terms by the vocabulary ID.
    $this->addFieldMapping('field_countries', '1')
         ->sourceMigration('Countries')
         ->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_topics', '2')
         ->sourceMigration('Topics')
         ->arguments(array('source_type' => 'tid'));
    */
    
  }
  
  // Modify the query to include location data
  // For more info, see:
  // http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture as well as
  // http://www.grasmash.com/article/migrate-classes-location-cck-address-field
  protected function query() {
    $query = parent::query();
    $query->leftJoin('location_instance', 'li', 'n.nid=li.nid AND n.vid = li.vid');
    $query->addField('li', 'lid');
    $query->leftJoin('location', 'l', 'li.lid=l.lid');
    $query->addField('l', 'street');
    $query->addField('l', 'additional');
    $query->addField('l', 'city');
    $query->addField('l', 'province');
    $query->addField('l', 'postal_code');
    $query->addField('l', 'country');
    $query->addField('l', 'latitude');
    $query->addField('l', 'longitude');
    $query->leftJoin('location_phone', 'lp', 'lp.lid=l.lid');
    $query->addField('lp', 'phone');
    $query->leftJoin('location_fax', 'lf', 'lf.lid=l.lid');
    $query->addField('lf', 'fax');
    $query->leftJoin('ding_library', 'dl', 'n.nid=dl.nid AND n.vid = dl.vid');
    $query->addField('dl', 'slug');
    return $query;
  }
}


class DingCampaignMigration extends DingNodeMigration {
  public function __construct() {
    
    $arguments = array(
      'description' => t('Migration of Campaigns from Ding1/D6'),
      'machine_name' => 'DingCampaign',
      'source_type' => 'campaign',
      'destination_type' => 'ding_campaign',
    );

    // Add to sourceFields before calling parent constructor. See http://www.acquia.com/blog/drupal-drupal-data-migration-part-2-architecture
    $this->sourceFields['campaign_image_alt'] = t('Alt text extracted from field_campaign_image_data');
    $this->sourceFields['campaign_image_title'] = t('Title text extracted from field_campaign_image_data');
    
    parent::__construct($arguments);

    $this->addFieldMapping('field_camp_text_full', 'body')
         ->description(t('body is mapped to both field_camp_text_full and field_camp_text_plain'));
    $this->addFieldMapping('field_camp_text_plain', 'body')
         ->description(t('body is mapped to both field_camp_text_full and field_camp_text_plain'));
    $this->addFieldMapping('field_camp_link', 'field_campaign_link');
    $this->addFieldMapping('field_camp_image:alt', 'campaign_image_alt');
    $this->addFieldMapping('field_camp_image:title', 'campaign_image_title');
    $this->addFieldMapping(NULL, 'field_campaign_image:data')
         ->description(t('Split this into campaign_image_alt and campaign_image_title for D7'));
    $this->addFieldMapping(NULL, '2')
         ->description(t('Ding2 does not employ the Tags taxonomy (taxonomy "2") for campaigns'));
    $this->addFieldMapping(NULL, 'field_campaign_link:title')
         ->description(t('Ding2 does not have title data on the campaign links'));
    $this->addFieldMapping(NULL, 'field_campaign_link:attributes')
         ->description(t('Ding2 does not have attribute data on the campaign links'));
    $this->addFieldMapping('field_camp_weight', 'campaign_weight');
    $this->addFieldMapping('field_camp_settings', 'campaign_type')
         ->description(t('Handled in prepareRow of DingCampaignMigration'));
    $this->addFieldMapping('field_camp_link:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_camp_text_full:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_camp_text_plain:language')
         ->defaultValue('und');
    $this->addFieldMapping('field_camp_text_full:format')
         ->defaultValue('ding_wysiwyg');
    $this->addFieldMapping(NULL, 'field_campaign_image:list');
         
    // Map the image field to the new image field, base this on the Files migration
    // to map the files to the imported files table.
    $this->addFieldMapping('field_camp_image', 'field_campaign_image')
         ->sourceMigration('Files');
    $this->addFieldMapping('field_camp_image:file_class')
         ->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_camp_image:language')
         ->defaultValue('und');

    // This is a shortcut you can use to mark several destination fields as DNM
    // at once
    /*
    $this->addUnmigratedDestinations(array(
      '2',
    ));
    */
  }
  
  public function prepareRow($row) {
    // Always include this snippet, in case our parent class decides to ignore the row
    if (parent::prepareRow($row) === FALSE) {
       return FALSE;
    }
    
    // Get values from serialized data and populate the source fields defined in our constructor
    $campaign_image_data = unserialize($row->field_campaign_image_data);
    $row->campaign_image_alt = $campaign_image_data['alt'];
    $row->campaign_image_title = $campaign_image_data['title'];
    
    // Modify text in campaign_type
    $row->campaign_type = str_replace('-only', '', $row->campaign_type);
  }
  
  // Modify the query to include ding_campaign data
  protected function query() {
    $query = parent::query();
    $query->leftJoin('ding_campaign', 'dc', 'n.nid=dc.nid AND n.vid = dc.vid');
    $query->addField('dc', 'campaign_weight');
    $query->addField('dc', 'campaign_type');
    return $query;
  }
}


class DingPageMigration extends DingNodeMigration {
  public function __construct() {
    
    $arguments = array(
      'description' => t('Migration of Campaigns from Ding1/D6'),
      'machine_name' => 'DingPage',
      'source_type' => 'page',
      'destination_type' => 'ding_page',
      'dependencies' => array('DingTagsTerm'),
    );

    parent::__construct($arguments);

    $this->addFieldMapping('field_ding_page_tags', '2')
         ->sourceMigration('DingTagsTerm')
         ->arguments(array('source_type' => 'tid'));
  }
}


class DingNewsMigration extends DingNodeMigration {
  public function __construct() {
    
    $arguments = array(
      'description' => t('Migration of Blogposts/News from Ding1/D6'),
      'machine_name' => 'DingNews',
      'source_type' => 'article',
      'destination_type' => 'ding_news',
      'dependencies' => array('DingNewsCategoryTerm'),
    );

    parent::__construct($arguments);

    $this->addFieldMapping('field_ding_news_category', '1')
         ->sourceMigration('DingNewsCategoryTerm')
         ->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_ding_news_tags', '2')
         ->sourceMigration('DingTagsTerm')
         ->arguments(array('source_type' => 'tid'));
  }
}


class DingEventMigration extends DingNodeMigration {
  public function __construct() {
    
    $arguments = array(
      'description' => t('Migration of Events from Ding1/D6'),
      'machine_name' => 'DingEvent',
      'source_type' => 'event',
      'destination_type' => 'ding_event',
      'dependencies' => array('DingEventCategoryTerm','DingEventTargetTerm'),
    );

    parent::__construct($arguments);

    //$this->addFieldMapping('field_ding_library_body', 'body');    
  }
}