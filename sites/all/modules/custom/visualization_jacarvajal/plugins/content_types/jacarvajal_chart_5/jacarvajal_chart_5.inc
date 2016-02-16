<?php
/**
 * @file
 * Definition of Jacarvajal Gráfica 5 Block.
 */

if (module_exists('visualization_jacarvajal')) {
  // Plugin definition.
  $plugin = [
    'title' => t('Jacarvajal Chart 5'),
    'category' => t('Jacarvajal'),
    'render callback' => 'jacarvajal_chart_5',
  ];
}

/**
 * Render callback.
 */
function jacarvajal_chart_5($subtype, $conf, $args, $context) {
  // Generate block content
  $block = new stdClass();
  $block->title = t('Content - Percent by content type');
  $block->content = _jacarvajal_chart5_content();

  return $block;
}

/**
 * Helper function to generate chart content
 */
function _jacarvajal_chart5_content() {
  // Color list. These colours will be used in order.
  $color_list = [
    '#6BBA9A',
    '#EED145',
    '#6097D7',
    '#1A3552',
    '#FB7049',
    '#97A52C',
    '#8B59B2',
  ];

  // Obtain data to generate the chart
  $result = _get_count_content_types();

  if (empty($result)) {
    $content = t('There are no published content yet.');
  }
  else {
    // Create array to manage data and fill it with query data
    foreach ($result as $value) {
      // Get content type name (not machine name)
      $data[$value->type]['content_type_name'] = node_type_get_name($value->type);
      $data[$value->type]['count'] = $value->count;
    }

    // Remove states with no data
    // If the state has data, give it a color
    foreach ($data as $key => $value) {
      if (empty($value['count'])) {
        unset($data[$key]);
      }
      else {
        $data[$key]['color'] = array_shift($color_list);
      }
    }

    // Add dpm() to show $data format
    dpm($data, '$data');

    // Fill options to generate the chart. This time, we use extra options,
    // added by highcharts plugin customization in visualization module (patching it)
    $options = [
      'type' => 'pie',
      'data' => $data,
      'dataLabelsFormat' => '{point.percentage:.1f}%',
      'innerSize' => '45%',
      'title' => t('Content - Percent by content type'),
      'fields' => [
        'count' => [
          'label' => t('Contents'),
          'enabled' => TRUE,
        ],
      ],
      'xAxis' => [
        'labelField' => 'content_type_name',
      ],
    ];

    $content['pie'] = [
      '#theme' => 'visualization',
      '#options' => $options,
    ];
  }

  return $content;
}

/**
 * Helper function to get count of content types
 * This function is used in _jacarvajal_chart5_content()
 */
function _get_count_content_types() {
  $query = db_select('node', 'n');
  $query->addExpression("COUNT(n.type)", 'count');
  $query->fields('n', ['type']);
  $query->condition('status', [1]);
  $query->groupBy('type');

  return $query->execute()->fetchAll();
}
