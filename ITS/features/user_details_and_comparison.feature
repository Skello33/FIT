Feature: Products details and comparison

  Scenario: Display detailed product specifications
    Given a web browser is at "Monitors" overview page
    When the user clicks on "Apple Cinema 30"
    Then product details for "Apple Cinema 30" are shown

  Scenario: Add product to compare
    Given a web browser is at "Apple Cinema 30" details page
    When the user clicks on Compare this Product
    Then the product is added to product comparison

#  Scenario: Display products comparison
#    Given a web browser is at products overview page
#    When the user clicks on Product Compare
#    Then product comparison is shown
#
#  Scenario: Remove product from comparison
#    Given a web browser is at product comparison page
#    And the product comparison contains product "Apple Cinema 30"
#    When the user clicks on "Remove" button on the bottom of page under product "Apple Cinema 30"
#    Then product comparison is shown without "Apple Cinema 30"
