# -*- coding: utf-8 -*-
# this file is released under public domain and you can use without limitations

# ----------------------------------------------------------------------------------------------------------------------
# this is the main application menu add/remove items as required
# ----------------------------------------------------------------------------------------------------------------------

response.menu = [
    (T('Home'), False, URL('default', 'index'), [])
]

# ----------------------------------------------------------------------------------------------------------------------
# provide shortcuts for development. you can remove everything below in production
# ----------------------------------------------------------------------------------------------------------------------

if not configuration.get('app.production'):
    _app = request.application
    response.menu += [
        (T('Notifications'), False, '#', [
            (T('List'), False, URL('default', 'notif_list')),
            (T('Calendar'), False, URL('default', 'notif_calendar')),
        ]),
        (T('Devices'), False, URL('default', 'devices')),
    ]
