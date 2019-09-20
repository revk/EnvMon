// Environment graph from mariadb
// Copyright (c) 2019 Adrian Kennard, Andrews & Arnold Limited, see LICENSE file (GPL)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <popt.h>
#include <err.h>
#include <curl/curl.h>
#include <sqllib.h>
#include <axl.h>

int debug = 0;

int
main (int argc, const char *argv[])
{
   const char *sqlhostname = NULL;
   const char *sqldatabase = "env";
   const char *sqlusername = NULL;
   const char *sqlpassword = NULL;
   const char *sqlconffile = NULL;
   const char *sqltable = "env";
   const char *tag = NULL;
   const char *date = NULL;
   int svgwidth = 864;
   int svgheight = 500;
   int debug = 0;
   {                            // POPT
      poptContext optCon;       // context for parsing command-line options
      const struct poptOption optionsTable[] = {
         {"sql-conffile", 'c', POPT_ARG_STRING, &sqlconffile, 0, "SQL conf file", "filename"},
         {"sql-hostname", 'H', POPT_ARG_STRING, &sqlhostname, 0, "SQL hostname", "hostname"},
         {"sql-database", 'd', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &sqldatabase, 0, "SQL database", "db"},
         {"sql-username", 'U', POPT_ARG_STRING, &sqlusername, 0, "SQL username", "name"},
         {"sql-password", 'P', POPT_ARG_STRING, &sqlpassword, 0, "SQL password", "pass"},
         {"sql-table", 't', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &sqltable, 0, "SQL table", "table"},
         {"sql-debug", 'v', POPT_ARG_NONE, &sqldebug, 0, "SQL Debug"},
         {"svg-width", 0, POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &svgwidth, 0, "SVG width"},
         {"svg-height", 0, POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT, &svgheight, 0, "SVG height"},
         {"tag", 'i', POPT_ARG_STRING, &tag, 0, "Device ID", "tag"},
         {"date", 'D', POPT_ARG_STRING, &tag, 0, "Device ID", "tag"},
         {"debug", 'V', POPT_ARG_NONE, &debug, 0, "Debug"},
         POPT_AUTOHELP {}
      };

      optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
      //poptSetOtherOptionHelp (optCon, "");

      int c;
      if ((c = poptGetNextOpt (optCon)) < -1)
         errx (1, "%s: %s\n", poptBadOption (optCon, POPT_BADOPTION_NOALIAS), poptStrerror (c));

      if (poptPeekArg (optCon) || !tag)
      {
         poptPrintUsage (optCon, stderr, 0);
         return -1;
      }
      poptFreeContext (optCon);
   }
   SQL sql;
   sql_real_connect (&sql, sqlhostname, sqlusername, sqlpassword, sqldatabase, 0, NULL, 0, 1, sqlconffile);
   SQL_RES *res;
   if (date)
      res =
         sql_safe_query_store_free (&sql,
                                    sql_printf ("SELECT * FROM `%#S` WHERE `tag`=%#s AND `when` like '%#S%' ORDER BY `when`",
                                                sqltable, tag, date));
   else
      res =
         sql_safe_query_store_free (&sql,
                                    sql_printf
                                    ("SELECT * FROM `%#S` WHERE `tag`=%#s AND `when`>=date_sub(now(),interval 1 day) ORDER BY `when`",
                                     sqltable, tag));
   char *pathtemp = NULL;
   size_t sizetemp = 0;
   char *pathco2 = NULL;
   size_t sizeco2 = 0;
   char *pathrh = NULL;
   size_t sizerh = 0;
   FILE *ftemp = open_memstream (&pathtemp, &sizetemp);
   FILE *fco2 = open_memstream (&pathco2, &sizeco2);
   FILE *frh = open_memstream (&pathrh, &sizerh);
   char rhm = 'M',
      co2m = 'M',
      tempm = 'M';
   time_t start = 0;
   double tempmin = 1000,
      tempmax = -1000;
   double rhmin = -1000,
      rhmax = -1000;
   double co2min = -1000,
      co2max = -1000;
   while (sql_fetch_row (res))
   {
      const char *when = sql_colz (res, "when");
      const char *co2 = sql_colz (res, "co2");
      const char *rh = sql_colz (res, "rh");
      const char *temp = sql_colz (res, "temp");
      if (!start)
         start = xml_time (when);
      int x = (xml_time (when) - start);
      if (co2)
      {
         double v = strtod (co2,NULL);
         fprintf (fco2, "%c%d,%f", co2m, x, v);
         co2m = 'L';
         if (co2min <=-1000 || co2min > v)
            co2min = v;
         if (co2max <=-1000 || co2max < v)
            co2max = v;
      }
      if (rh)
      {
         double v = strtod (rh,NULL)*100;
         fprintf (frh, "%c%d,%f", rhm, x, v);
         rhm = 'L';
         if (rhmin <=-1000 || rhmin > v)
            rhmin = v;
         if (rhmax <=-1000 || rhmax < v)
            rhmax = v;
      }
      if (temp)
      {
         double v = strtod (temp, NULL)*100;
         fprintf (ftemp, "%c%d,%f", tempm, x, v);
         tempm = 'L';
         if (tempmin <= -1000 || tempmin > v)
            tempmin = v;
         if (tempmax <= -1000 || tempmax < v)
            tempmax = v;
      }
   }
   fclose (ftemp);
   fclose (fco2);
   fclose (frh);
   sql_free_result (res);
   xml_t svg = xml_tree_new ("svg");
   xml_element_set_namespace (svg, xml_namespace (svg, NULL, "http://www.w3.org/2000/svg"));
   xml_addf (svg, "@width", "%d", svgwidth);
   xml_addf (svg, "@height", "%d", svgheight);
   if (*pathtemp)
   {
      xml_t p = xml_element_add (svg, "path");
      xml_add (p, "@stroke", "red");
      xml_add (p, "@fill", "none");
      xml_addf (p, "@transform", "scale(%f,%f)translate(0,%f)", (double) svgwidth / 86400, (double) svgheight / (tempmax - tempmin),-tempmin);
      xml_add (p, "@d", pathtemp);
   }
   if (*pathco2)
   {
      xml_t p = xml_element_add (svg, "path");
      xml_addf (p, "@transform", "scale(%f,%f)translate(0,%f)", (double) svgwidth / 86400, (double) svgheight / (co2max - co2min),-co2min);
      xml_add (p, "@stroke", "green");
      xml_add (p, "@fill", "none");
      xml_add (p, "@d", pathco2);
   }
   if (*pathrh)
   {
      xml_t p = xml_element_add (svg, "path");
      xml_addf (p, "@transform", "scale(%f,%f)translate(0,%f)", (double) svgwidth / 86400, (double) svgheight / (rhmax - rhmin),-rhmin);
      xml_add (p, "@stroke", "blue");
      xml_add (p, "@fill", "none");
      xml_add (p, "@d", pathrh);
   }

   xml_write (stdout, svg);
   xml_tree_delete (svg);
   return 0;
}
