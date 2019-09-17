// Env sensor log to mariadb from MQTT
// Copyright (c) 2019 Adrian Kennard, Andrews & Arnold Limited, see LICENSE file (GPL)

#include <stdio.h>
#include <string.h>
#include <popt.h>
#include <err.h>
#include <malloc.h>
#include <sqllib.h>
#include <mosquitto.h>

int
main (int argc, const char *argv[])
{
   const char *sqlhostname = NULL;
   const char *sqldatabase = "env";
   const char *sqlusername = NULL;
   const char *sqlpassword = NULL;
   const char *sqlconffile = NULL;
   const char *sqltable = "env";
   const char *mqtthostname = "localhost";
   const char *mqttusername = NULL;
   const char *mqttpassword = NULL;
   const char *mqttappname = "Env";
   const char *mqttid = NULL;
   int debug = 0;
   {                            // POPT
      poptContext optCon;       // context for parsing command-line options
      const struct poptOption optionsTable[] = {
         {"sql-conffile", 'c', POPT_ARG_STRING, &sqlconffile, 0, "SQL conf file", "filename"},
         {"sql-hostname", 'd', POPT_ARG_STRING, &sqlhostname, 0, "SQL hostname", "hostname"},
         {"sql-database", 'd', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &sqldatabase, 0, "SQL database", "db"},
         {"sql-username", 'U', POPT_ARG_STRING, &sqlusername, 0, "SQL username", "name"},
         {"sql-password", 'P', POPT_ARG_STRING, &sqlpassword, 0, "SQL password", "pass"},
         {"sql-table", 't', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &sqltable, 0, "SQL table", "table"},
         {"sql-debug", 'v', POPT_ARG_NONE, &sqldebug, 0, "SQL Debug"},
         {"mqtt-hostname", 'h', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &mqtthostname, 0, "MQTT hostname", "hostname"},
         {"mqtt-username", 'u', POPT_ARG_STRING, &mqttusername, 0, "MQTT username", "username"},
         {"mqtt-password", 'p', POPT_ARG_STRING, &mqttpassword, 0, "MQTT password", "password"},
         {"mqtt-appname", 'a', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &mqttappname, 0, "MQTT appname", "appname"},
         {"mqtt-id", 0, POPT_ARG_STRING, &mqttid, 0, "MQTT id", "id"},
         {"debug", 'V', POPT_ARG_NONE, &debug, 0, "Debug"},
         POPT_AUTOHELP {}
      };

      optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);

      int c;
      if ((c = poptGetNextOpt (optCon)) < -1)
         errx (1, "%s: %s\n", poptBadOption (optCon, POPT_BADOPTION_NOALIAS), poptStrerror (c));

      if (poptPeekArg (optCon))
      {
         poptPrintUsage (optCon, stderr, 0);
         return -1;
      }
      poptFreeContext (optCon);
   }
   int e = mosquitto_lib_init ();
   if (e)
      errx (1, "MQTT init failed %s", mosquitto_strerror (e));
   struct mosquitto *mqtt = mosquitto_new (mqttid, 1, NULL);
   if (mqttusername)
   {
      e = mosquitto_username_pw_set (mqtt, mqttusername, mqttpassword);
      if (e)
         errx (1, "MQTT auth failed %s", mosquitto_strerror (e));
   }
   void connect (struct mosquitto *mqtt, void *obj, int rc)
   {
      obj = obj;
      rc = rc;
      char *sub = NULL;
      asprintf (&sub, "info/%s/#", mqttappname);
      int e = mosquitto_subscribe (mqtt, NULL, sub, 0);
      if (e)
         errx (1, "MQTT subscribe failed %s (%s)", mosquitto_strerror (e), sub);
      if (debug)
         warnx ("MQTT Sub %s", sub);
      free (sub);
   }
   void disconnect (struct mosquitto *mqtt, void *obj, int rc)
   {
      obj = obj;
      rc = rc;
   }
   void message (struct mosquitto *mqtt, void *obj, const struct mosquitto_message *msg)
   {
      obj = obj;
      char *topic = msg->topic;
      if (debug)
         warnx ("MQTT %s", topic);
   }
   mosquitto_connect_callback_set (mqtt, connect);
   mosquitto_disconnect_callback_set (mqtt, disconnect);
   mosquitto_message_callback_set (mqtt, message);
   e = mosquitto_connect (mqtt, mqtthostname, 1883, 60);
   if (e)
      errx (1, "MQTT connect failed (%s) %s", mqtthostname, mosquitto_strerror (e));
   SQL sql;
   sql_real_connect (&sql, sqlhostname, sqlusername, sqlpassword, sqldatabase, 0, NULL, 0, 1, sqlconffile);
   e = mosquitto_loop_forever (mqtt, -1, 1);
   if (e)
      errx (1, "MQTT loop failed %s", mosquitto_strerror (e));
   mosquitto_destroy (mqtt);
   mosquitto_lib_cleanup ();
   sql_close (&sql);
   return 0;
}
