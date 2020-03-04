Feature: Searching for products

#  Scenario: Product search by name
#    Given a web browser is at Opencart homepage
#    When the user enters "iPhone" into the search bar
#    Then products related to "iPhone" are displayed on the results page
#
#  Scenario: Categories product search
#    Given a web browser is at Opencart homepage
#    When the user hovers over "Components"
#    And the user clicks on "Monitors"
#    Then monitors for sale are shown

  Scenario: Products sort
    Given a web browser is at products overview page
    When the user selects Rating(Highest) from the "Sort by" dropdown menu
    Then products sorted from highest to lowest rating are shown on the page

  Scenario: Product show count
    Given a web browser is at products overview page
    When the user selects "25" from the "Show" dropdown menu
    Then 25 products are shown on the page
