import yaml

def eval_constructor(loader, node):
  return eval(loader.construct_scalar(node))

yaml.add_constructor(u'!eval', eval_constructor)

some_value = '123'

config = yaml.load("""
version: 1
formatters:
  simple:
    format: '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
handlers:
  logfile:
    class: logging.handlers.TimedRotatingFileHandler
    level: DEBUG
    filename: !eval some_value
    backupCount: 5
    formatter: simple
""")

print( config['handlers']['logfile']['filename'])