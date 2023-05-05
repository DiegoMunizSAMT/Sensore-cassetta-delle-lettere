# -*- coding: utf-8 -*-
import logging, logging.handlers
import os

'''
this model is name A_log.py so it gets loaded first
'''

def get_configured_logger(name=None):
    new_handlers = []
    
    logger = logging.getLogger(name)
    logger.setLevel(logging.DEBUG)
    
    formatter = "%(asctime)s | %(levelname)s | %(process)s | %(thread)s | %(filename)s | %(funcName)s():%(lineno)d | %(message)s"
    
    handler = logging.handlers.RotatingFileHandler(os.path.join(request.folder,'private/app.log'), maxBytes = 1024*1024*10, backupCount = 6)
    handler.setFormatter(logging.Formatter(formatter))
    handler.setLevel(logging.INFO)
    new_handlers.append(handler)

    handler_d = logging.handlers.RotatingFileHandler(os.path.join(request.folder,'private/app_debug.log'), maxBytes = 1024*1024*10, backupCount = 2)
    handler_d.setFormatter(logging.Formatter(formatter))
    handler_d.setLevel(logging.DEBUG)    
    new_handlers.append(handler_d)
    
    logger.handlers.clear()

    for h in new_handlers:
        logger.addHandler(h)

    '''
    if (len(logger.handlers) == 0):
        # This logger has no handlers, so we can assume it hasn't yet been configured
        # Create RotatingFileHandler
        formatter = "%(asctime)s | %(levelname)s | %(process)s | %(thread)s | %(filename)s | %(funcName)s():%(lineno)d | %(message)s"


        logger.addHandler(handler)
        logger.addHandler(handler_d)
        logger.setLevel(logging.DEBUG)

        # Test entry:
        logger.debug(name + ' logger created')
    else:
        # Test entry:
        message = f'logger {logger} already has {len(logger.handlers)} handlers: {logger.handlers}'
        logging.info(message)
        
        fh = open( os.path.join(request.folder,'private/app_debug.log'), 'w' )
        fh.write(message)
        fh.close()
        pass
    '''
    return logger

def raw_log(message):
    fh = open( os.path.join(request.folder,'private/app_debug.log'), 'w' )
    fh.write(message)
    fh.close()    

# Assign application logger to a global var  
logger = get_configured_logger(request.application)

from gluon import current
current.logger = logger
