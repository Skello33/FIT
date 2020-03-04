from selenium import webdriver
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities


def before_all(context):
    capabilities = {'browserName': 'firefox', 'marionette': 'true',
          'javascriptEnabled': 'true'}
    context.browser = webdriver.Remote(command_executor='http://mys01.fit.vutbr.cz:4444/wd/hub',
                                       desired_capabilities=capabilities)
    # context.browser = webdriver.Remote(command_executor='localhost:9515',
    #                                    desired_capabilities=dp)
    context.browser.implicitly_wait(15)
    context.base_url = "http://mys01.fit.vutbr.cz:8061/"


def after_all(context):
    context.browser.quit()
