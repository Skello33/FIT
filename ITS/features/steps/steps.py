from behave import *
from selenium import *
from selenium import webdriver
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities

# @given('we have behave installed')
# def step_impl(context):
#     pass
#
#
# @when('we implement a test')
# def step_impl(context):
#     assert True is not False
#
#
# @then('behave will test it for us!')
# def step_impl(context):
#     assert context.failed is False


@given('a web browser is at product details page')
def step_impl(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8061/index.php?route=product/product&path=25_28&product_id=42')
    context.browser.set_window_size(1853, 1025)


@when("the user fills all required fields with valid data")
def step_impl(context):

    context.browser.find_element_by_css_selector('.radio:nth-child(1) > label').click()
    context.browser.find_element_by_css_selector('.checkbox:nth-child(1) > label').click()
    context.browser.find_element_by_id('input-option217').click()
    context.browser.find_element_by_id('input-option209').click()
    context.browser.find_element_by_id('input-option209').send_keys('text')


@step('the user clicks on "Add to Cart" button')
def step_impl(context):
    context.browser.find_element_by_id('button-cart').click()


@then("the product is added to users cart")
def step_impl(context):
    text = context.browser.find_element_by_css_selector('.alert.alert-success').text.split(":")[0]
    assert text == 'Success'


@given("a web browser is at Opencart homepage")
def step_impl(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8061/')
    context.browser.set_window_size(1853, 1025)


@when("the user clicks on shopping cart icon in the upper right side of the page")
def step_impl(context):
    context.browser.find_element_by_id('cart-total').click()


@then("the contents of shopping cart are displayed")
def step_impl(context):
    text = context.browser.find_element_by_id('cart-total').text.split()[0]
    assert text != 0

#
# @given("a web browser is at shopping cart overview page")
# def step_impl(context):
#     context.browser.get('http://mys01.fit.vutbr.cz:8061/index.php?route=checkout/cart')
#     context.browser.set_window_size(1853, 1025)
#
#
# @step('the shopping cart contains product "Apple Cinema 30"')
# def step_impl(context):
#     """
#     :type context: behave.runner.Context
#     """
#     raise NotImplementedError(u'STEP: And the shopping cart contains product "Samsung SyncMaster 941BW"')
#
#
# @when('the user clicks on "Remove" button of the product "Apple Cinema 30"')
# def step_impl(context):
#     """
#     :type context: behave.runner.Context
#     """
#     raise NotImplementedError(
#         u'STEP: When the user clicks on "Remove" button of the product "Apple Cinema 30"')
#
#
# @then('the product "Apple Cinema 30" is removed from the shopping cart')
# def step_impl(context):
#     """
#     :type context: behave.runner.Context
#     """
#     raise NotImplementedError(u'STEP: Then the product "Samsung SyncMaster 941BW" is removed from the shopping cart')
#
#
# @step("shopping cart overview page without the removed product is shown")
# def step_impl(context):
#     """
#     :type context: behave.runner.Context
#     """
#     raise NotImplementedError(u'STEP: And shopping cart overview page without the removed product is shown')


@given("a web browser is at products overview page")
def step_impl(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8061/index.php?route=product/category&path=25_28')
    context.browser.set_window_size(1853, 1025)


@when('the user selects Rating(Highest) from the "Sort by" dropdown menu')
def step_impl(context):
    context.browser.find_element_by_id('input-sort').click()
    dropdown = context.browser.find_element_by_id('input-sort')
    dropdown.find_element_by_xpath("//option[. = 'Rating (Highest)']").click()


@then("products sorted from highest to lowest rating are shown on the page")
def step_impl(context):
    text = context.browser.find_element_by_xpath('/html/body/div[2]/div/div/div[3]/div[1]').text.split("\n")[0]
    assert text == 'Samsung SyncMaster 941BW'
    text2 = context.browser.find_element_by_xpath('/html/body/div[2]/div/div/div[3]/div[2]').text.split("\n")[0]
    assert text2 == 'Apple Cinema 30\"'


@when('the user selects "25" from the "Show" dropdown menu')
def step_impl(context):
    context.browser.find_element_by_id('input-limit').click()
    dropdown = context.browser.find_element_by_id('input-limit')
    dropdown.find_element_by_xpath("//option[. = '25']").click()


@then("25 products are shown on the page")
def step_impl(context):
    text = context.browser.find_element_by_css_selector('.row:nth-child(8) > .text-right').text.split("of ")[1].split()[
        0]
    assert int(text) <= 25


@given('a web browser is at "Monitors" overview page')
def step_impl(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8061/index.php?route=product/category&path=25_28')
    context.browser.set_window_size(1853, 1025)


@when('the user clicks on "Apple Cinema 30"')
def step_impl(context):
    context.browser.find_element_by_xpath("/html/body/div[2]/div/div/div[3]/div[1]").click()


@then('product details for "Apple Cinema 30" are shown')
def step_impl(context):
    text = context.browser.find_element_by_xpath("/html/body/div[2]/ul/li[4]").text
    assert text == 'Apple Cinema 30\"'


@given('a web browser is at "Apple Cinema 30" details page')
def step_impl(context):
    context.browser.get('http://mys01.fit.vutbr.cz:8061/index.php?route=product/category&path=25_28')
    context.browser.set_window_size(1853, 1025)
    context.browser.find_element_by_xpath("/html/body/div[2]/div/div/div[3]/div[1]").click()
    text = context.browser.find_element_by_xpath("/html/body/div[2]/ul/li[4]").text
    assert text == 'Apple Cinema 30\"'


@when("the user clicks on Compare this Product")
def step_impl(context):
    context.browser.find_element_by_xpath("/html/body/div[2]/div/div/div[1]/div[2]/div[1]/button[2]").click()


@then("the product is added to product comparison")
def step_impl(context):
    text = context.browser.find_element_by_css_selector('.alert.alert-success').text.split(":")[0]
    assert text == 'Success'
