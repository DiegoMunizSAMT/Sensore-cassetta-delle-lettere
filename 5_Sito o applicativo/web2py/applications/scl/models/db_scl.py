db.define_table('notification',
               Field('content_value', 'text')
               )

db.define_table('sensor',
               Field('hardware_type', 'string'),
               Field('recored_value', 'boolean'),
               Field('arrival_time', 'datetime'),
               Field('notif_id', 'reference notification'))

db.define_table('device',
               Field('model', 'string'))

db.define_table('device_has_sensor',
               Field('dev_id', 'reference device'),
               Field('snsr_id', 'reference sensor'))

db.define_table('user_has_device',
               Field('user_id', 'reference auth_user'),
               Field('dev_id', 'reference device'))

db.define_table('notification_notifies_user',
               Field('notif_id', 'reference notification'),
               Field('user_id', 'reference auth_user'))
