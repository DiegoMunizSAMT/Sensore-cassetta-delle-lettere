# -*- coding: utf-8 -*-
# -------------------------------------------------------------------------
# This is a sample controller
# this file is released under public domain and you can use without limitations
# -------------------------------------------------------------------------

# ---- example index page ----

import datetime


def index():
    response.flash = T("First time on SCL?")
    logger.debug('someone visited this page')
    return {}

@auth.requires_login()
def notif_list():
    grid = SQLFORM.smartgrid(
        db.recorded_value,
        editable=False,
        create=False,
        searchable=False,
        details=False,
        csv=False,
        maxtextlength=40
        )
    return {'grid':grid}

@auth.requires_login()
def notif_calendar():
    from datetime import date
    today = date.today().isoformat()

    #user_has_device -> recorded_value
    usersHaveDevices = db(db.user_has_device.user_id==auth.user.id).select()
    
    arrivalTimes = []
    
    for row in usersHaveDevices:
        devices = db(db.recorded_value.device_id==row.dev_id).select()
        for row2 in devices:
            arrivalTimes.append(row2.arrival_time)

    events = ""
    index = 0
    for row in arrivalTimes:
        events += "{" + os.linesep
        events += "title: '" + "Notification" + "'," + os.linesep
        events += "start: '" + str(row).split(' ')[0] + "'" + os.linesep
        if index < len(arrivalTimes) - 1:
            events += "}," + os.linesep
        else:
            events += "}" + os.linesep
        index = index + 1;

    return {'events':events, 'today':today}

#@auth.requires_login()
def devices():
    grid = SQLFORM.smartgrid(
        db.device,
        csv=False,
        maxtextlength=40
        )
    return {'grid':grid}

@request.restful()
def api():
    logger.debug(f'request received: {request}')

    def GET(tablename, id):
        if not tablename == 'device':
            logger.error(f'invalid tablename {tablename}')
            raise HTTP(400)
        return dict(device = db.device(id))

    def POST(tablename, **vars):
        logger.debug(f'vars: {vars}')
        logger.debug(f'request body: {request.body}')
        try:
            logger.debug(f'request body (decode): {(request.body).decode("utf-8")}' )
        except:
            pass
        
        try:
            logger.debug(f'request body (getvalue): {(request.body).getvalue().decode("utf-8")}' )
        except:
            pass
        
        try:
            logger.debug(f'request body (read): {(request.body).read(100).decode("utf-8")}' )
        except:
            pass
        
        logger.debug(f'request vars: {request.vars}')
        out = None
        if not tablename == 'recorded_value':
            logger.error(f'invalid tablename {tablename}')
            raise HTTP(400)

        if vars:
            #check device with api_key
            device_id = get_device_id(vars['api_key']) if 'api_key' in vars else None
            if not device_id:
                message = f'invalid key {vars["api_key"]}'
                logger.error(message)
                raise HTTP(401, message)

            record_id = save_recorded_value(vars, device_id)
            return dict(record_id = record_id)
        else:
            raise HTTP(401, 'no api_key')


    def PUT(*args, **vars):
        return dict()

    def DELETE(*args, **vars):
        return dict()

    return locals()

def get_device_id(api_key):
    rows = db(db.device.api_key == api_key).select()
    if rows:
        print(rows),
        return rows[0].id
    else:
        return False

def save_recorded_value(rawdata, device_id):
    record_ids = []
    for key, value in rawdata.items():
        if key == 'api_key':
            pass
        else:
            record_id = db.recorded_value.insert(
                device_id = device_id,
                recorded_action = key,
                recorded_value = value,
                arrival_time = datetime.datetime.now()
            )
            record_ids.append( record_id )

    return record_ids

# ---- API (example) -----
@auth.requires_login()
def api_get_user_email():
    if not request.env.request_method == 'GET': raise HTTP(403)
    return response.json({'status':'success', 'email':auth.user.email})

# ---- Smart Grid (example) -----
@auth.requires_membership('admin') # can only be accessed by members of admin groupd
def grid():
    response.view = 'generic.html' # use a generic view
    tablename = request.args(0)
    if not tablename in db.tables: raise HTTP(403)
    grid = SQLFORM.smartgrid(db[tablename], args=[tablename], deletable=False, editable=False)
    return dict(grid=grid)

# ---- Embedded wiki (example) ----
def wiki():
    auth.wikimenu() # add the wiki to the menu
    return auth.wiki()

# ---- Action for login/register/etc (required for auth) -----
def user():
    """
    exposes:
    http://..../[app]/default/user/login
    http://..../[app]/default/user/logout
    http://..../[app]/default/user/register
    http://..../[app]/default/user/profile
    http://..../[app]/default/user/retrieve_password
    http://..../[app]/default/user/change_password
    http://..../[app]/default/user/bulk_register
    use @auth.requires_login()
        @auth.requires_membership('group name')
        @auth.requires_permission('read','table name',record_id)
    to decorate functions that need access control
    also notice there is http://..../[app]/appadmin/manage/auth to allow administrator to manage users
    """
    return dict(form=auth())

# ---- action to server uploaded static content (required) ---
@cache.action()
def download():
    """
    allows downloading of uploaded files
    http://..../[app]/default/download/[filename]
    """
    return response.download(request, db)
