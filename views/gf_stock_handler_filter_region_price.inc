<?php

/**
 * @uses views_handler_field_numeric
 */
class gf_stock_handler_filter_region_price extends views_handler_filter_numeric{

  public function construct() {
    parent::construct();
    // $this->additional_fields['currency']['field'] = 'currency';
  }


  public function query() {
    $this->ensure_my_table();
    $nid = $this->relationship ? $this->relationship . '_nid' :'node.nid';
    $region = gf_stock_get_current_region();
    $price_id = variable_get('gf_stock_region_price_' . $region);
    $currency = ($region == GF_STOCK_REGION_CN) ? GF_STOCK_CURRENCY_CN : GF_STOCK_CURRENCY_RU;
    // Значение регионального ценника выводим подзапросом. 
    // Ибо сраный вьюс так и не научился работать с составными
    // первичными ключами. Или я в нем это тупо не нашел(что врядли).
    $expression = "(SELECT price FROM gf_stock_prices 
      WHERE nid = {$nid} AND price_id = '$price_id' AND currency = '$currency')";

    $info = $this->operators();
    if (!empty($info[$this->operator]['method'])) {
      $this->{$info[$this->operator]['method']}($expression);
    }
  }


  /**
   * Сравнение по вхождение(или невхождение) в диапазон значений.
   */
  function op_between($expression) {
    $op = ($this->operator == 'between') ? 'BETWEEN' : 'NOT BETWEEN';
    $this->query->add_where_expression($this->options['group'], 
      "$expression $op :min AND :max",
      [
        ':min' => $this->value['min'],
        ':max' => $this->value['max']
      ]);
  }

  /**
   * Простое сравнение (типа больше/меньше).
   */
  function op_simple($expression) {
    $this->query->add_where_expression($this->options['group'], 
      "$expression $this->operator :value",
      [':value' => $this->value["value"]]);
  }

  /**
   * Пустое значение.
   */
  function op_empty($expression) {
    $op = ($this->operator == 'empty') ? 'IS NULL' : 'IS NOT NULL';

    $this->query->add_where_expression($this->options['group'], 
      "$expression $op");
  }

  /**
   * Соответствие по регулярке.
   */
  function op_regex($expression) {
    $this->query->add_where_expression($this->options['group'], 
      "$expression RLIKE :value", [':value' => $this->value["value"]]);
  }
}
