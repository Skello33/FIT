Feature: Product shopping

  Scenario: Add product to cart
    Given a web browser is at product details page
    When the user fills all required fields with valid data
    And the user clicks on "Add to Cart" button
    Then the product is added to users cart

  Scenario: Display cart contents
    Given a web browser is at Opencart homepage
    When the user clicks on shopping cart icon in the upper right side of the page
    Then the contents of shopping cart are displayed

#  Scenario: Remove product from shopping cart
#    Given a web browser is at shopping cart overview page
#    And the shopping cart contains product "Apple Cinema 30"
#    When the user clicks on "Remove" button of the product "Apple Cinema 30"
#    Then the product "Apple Cinema 30" is removed from the shopping cart
#    And shopping cart overview page without the removed product is shown
