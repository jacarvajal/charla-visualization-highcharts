<?php
/**
 * @file
 * Definition of Jacarvajal Gráfica 4 Block.
 */

if (module_exists('visualization_jacarvajal')) {
  // Plugin definition.
  $plugin = [
    'title' => t('Jacarvajal Chart 4'),
    'category' => t('Jacarvajal'),
    'render callback' => 'jacarvajal_chart_4',
  ];
}

/**
 * Render callback.
 */
function jacarvajal_chart_4($subtype, $conf, $args, $context) {
  // Generate block content
  $block = new stdClass();
  $block->title = t('Cheapest product by type');
  $block->content = _jacarvajal_chart4_content();

  return $block;
}

/**
 * Helper function to generate chart content
 */
function _jacarvajal_chart4_content() {
  // Obtain data to generate the chart
  $result = _get_product_min_price_by_type();

  // Parse data to a 'chart options' array
  if (empty($result)) {
    $content = t("There are no published products yet.");
  }
  else {
    // Get product type labels
    $allowed_values = list_allowed_values(field_info_field('field_product_type'));

    // Create array to manage data
    foreach ($allowed_values as $key => $value) {
      $data[$key]['product_type_label'] = $value;
      $data[$key]['product_type_min_price'] = 0;
    }

    // Fill the data with query data
    foreach ($result as $key => $value) {
      $data[$value->field_product_type_value]['product_type_min_price'] = $value->min_price;
    }

    // Remove states with no data
    foreach ($data as $key => $value) {
      if (empty($value['product_type_min_price'])) {
        unset($data[$key]);
      }
    }

    // Add dpm() to show $data format
    dpm($data, '$data');

    // Fill options to generate the chart
    $options = [
      'type' => 'bar',
      'data' => $data,
      'title' => t('Cheapest product by type'),
      'fields' => [
        'product_type_min_price' => [
          'label' => t('Minimum price'),
          'enabled' => TRUE,
        ],
      ],
      'xAxis' => [
        'labelField' => 'product_type_label',
      ],
    ];

    $content['bar'] = [
      '#theme' => 'visualization',
      '#options' => $options,
    ];
  }

  return $content;
}

/**
 * Helper function to get cheapest product by type
 * This function is used in _jacarvajal_chart4_content()
 */
function _get_product_min_price_by_type() {
  $query = db_select('field_data_field_price', 'p');
  $query->join('field_data_field_product_type', 'pt', 'pt.entity_id = p.entity_id');
  $query->join('node', 'n', 'n.nid = p.entity_id');
  $query->fields('pt', ['field_product_type_value']);
  $query->addExpression("MIN(p.field_price_value)", 'min_price');
  $query->condition('status', [1]);
  $query->condition('type', ['product']);
  $query->groupBy('pt.field_product_type_value');

  return $query->execute()->fetchAll();
}
