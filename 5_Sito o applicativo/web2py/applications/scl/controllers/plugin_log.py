'''
log display plugin
use in view with a call like this

{{=LOAD('plugin_log', 'index.load', ajax=True)}}

and customize the args in index for debug or not

'''
import os

#@auth.requires_membership('admin')
def index():
    response.title = 'Logs'
    clear_log =A(T('clear log'), _class='button btn btn-info', _onclick = "ajax('%s', [], 'clear_log')" % (URL(c='log', f='clear_app_log_data', args=['debug'])))

    #log_data = LOAD(c='plugin_log', f='app_log_data', args=['debug'], ajax=True, target='log_container', times='infinity', timeout=3000)
    filedata = _read_app_log_data('app_debug.log')
    log_data = _format_log_data(filedata)

    return {'clear_log':clear_log, 'log_data':log_data}

#@auth.requires_membership('admin')
def app_log_data():
    load_debug = True if 'debug' in request.args else False
    #http://www.web2py.com/books/default/chapter/29/04/the-core#request
    import os
    import os.path

    logname = 'app.log'
    if load_debug:
        logname = 'app_debug.log'

    filedata = _read_app_log_data(logname)
    #logger.debug('log_data: %s' % filedata)
    formatted_log_data = _format_log_data(filedata)
    return formatted_log_data

def _read_app_log_data(logname):
    log_file_path = os.path.join(request.folder, 'private', logname)
    if os.path.exists(log_file_path):
        logfile = open(log_file_path)
        filedata = logfile.readlines()
    else:
        filedata = []

    return filedata

def _format_log_data(data):
    formatted_log_data = UL(_class='unstyled list-unstyled')
    for line in data:
        formatted_log_data.append(LI(line))
    return formatted_log_data

#@auth.requires_membership('admin')
def clear_app_log_data():
    load_debug = True if 'debug' in request.args else False

    logger.warn('clear_log')
    import os
    import os.path
    logname = 'app.log'
    if load_debug:
        logname = 'app_debug.log'

    log_file_path = os.path.join(request.folder, 'private', logname)
    if os.path.isfile(log_file_path):
        try:
            #os.remove(log_file_path)   #remove may not work because already in use
            logfile = open(log_file_path, 'w')
            logfile.write('')
            logfile.close()
            logger.info('logfile cleared')
        except Exception:
            logger.exception('error deleting application log')
