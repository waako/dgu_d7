<?php

if (module_exists('apachesolr_search')) {
    $plugin = array(
        'single' => TRUE,
        'title' => t('DGU Search information'),
        'description' => t('Result count for the current SOLR search.'),
        'icon' => 'icon_search.png',
        'category' => t('Apache Solr Search'),
        'hook theme' => 'dgu_search_info_theme',
        //'render callback' => 'dgu_search_info_content_type_render',
        'defaults' => array(
            'title_override' => FALSE,
            'title_override_text' => '',
        ),
    );
}

/**
 * Delegated implementation of hook_theme().
 */
function dgu_search_info_theme(&$theme) {
    $theme['dgu_search_info'] = array(
        'path' => drupal_get_path('module', 'dgu_search') . '/templates',
        'template' => 'dgu_search_info',
    );
}

function dgu_search_preprocess(&$variables, $hook){

    switch ($hook){
      case 'dgu_search_info':
            $query = apachesolr_current_query(variable_get('apachesolr_default_environment', 'solr'));
            if ($query) {
                $searcher = $query->getSearcher();
                $response = apachesolr_static_response_cache($searcher);
            }
            if (empty($response)){
                return;
            }
            $variables['found'] = $response->response->numFound;
            $variables['start'] = $response->response->start + 1;
            $variables['end'] = $response->response->start + count($response->response->docs);

            $content_type = 'Result';
            $entity = '';
            foreach($query->getFilters() as $filter){
                if (isset($filter['#name']) && $filter['#name'] == 'entity_type'){
                    $entity = str_replace(array('"', "'"), '', $filter['#value']);
                }
                if (isset($filter['#name']) && $filter['#name'] == 'bundle'){
                    $content_type = $filter['#value'];
                }
            }

            if ($entity){
                $entity_info = entity_get_info($entity);
                if (!empty($entity_info['bundles'][$content_type]['label'])){
                    $content_type = $entity_info['bundles'][$content_type]['label'];
                }
            }

            $search_types = dgu_site_feature_get_plural_content_types();
            $variables['type'] = $search_types[$content_type] ? $search_types[$content_type] : 'Content results';
            break;
        default:
            # code...
            break;
    }
}


/**
 * Execute the search and render results.
 */
function dgu_search_dgu_search_info_content_type_render($subtype, $conf, $panel_args, $context) {
    $block = new stdClass();
    $block->module  = 'apachesolr_panels';
    $block->delta   = 'info';
    $block->title   = '';
    $block->content = theme('dgu_search_info');

    return $block;
}

/**
 * Empty form so we can have the default override title.
 */
function dgu_search_info_content_type_edit_form($form, &$form_state) {
    return $form;
}
