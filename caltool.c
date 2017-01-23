/***********
caltool.c

Functions for extracting and interpreting data from a Calendar structure
***********/

#include "caltool.h"
#include "calutil.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

//For qsort to compare strings
int compareStr (const void * one, const void * two);

//For qsort to compare dates
int compareDate (const void * one, const void * two);

//For printing syntax error messages and displaying proper use
void printMess (int error);

int main(int argc, char * argv[]){
    CalComp *pcomp = NULL;;
    CalComp * pcomp2 = NULL;
    CalStatus status;
    CalStatus status2;
    status.code = OK;
    status2.code = OK;
    struct tm * from = NULL;
    struct tm * to = NULL;
    time_t fromt;
    time_t tot;
    FILE * fp = NULL;

    int date;
    if (argc > 1 && (strcmp(argv[1], "-info") == 0
        || strcmp(argv[1], "-extract") == 0 || strcmp(argv[1], "-filter") == 0
        || strcmp(argv[1], "-combine") == 0)){
    if (strcmp(argv[1], "-info") == 0){
        if (argc == 2){
            status = readCalFile (stdin, &pcomp);
            if (status.code == OK){
                status = calInfo (pcomp, status.lineto, stdout);
            }
            else {
                freeCalComp(pcomp);
                pcomp = NULL;
            }
        }
        else {
            printMess (1);
        }
    }
    else if (strcmp(argv[1], "-extract") == 0){
        if (argc == 3){
            if (strcmp(argv[2], "e") == 0){
                status = readCalFile (stdin, &pcomp);
                if (status.code == OK){
                    status = calExtract (pcomp, OEVENT, stdout);
                }
                else {
                    freeCalComp(pcomp);
                    pcomp = NULL;
                }
            }
            else if (strcmp(argv[2], "x") == 0){
                status = readCalFile (stdin, &pcomp);
                if (status.code == OK){
                    status = calExtract (pcomp, OPROP, stdout);
                }
                else {
                    freeCalComp(pcomp);
                    pcomp = NULL;
                }
            }
            else {
                printMess(2);
            }
        }
        else {
            printMess(3);
        }
    }
    else if (strcmp(argv[1], "-filter") == 0){
        if (argc == 3){
            if (strcmp(argv[2], "e") == 0){
                status = readCalFile (stdin, &pcomp);
                if (status.code == OK){
                    status = calFilter (pcomp, OEVENT, 0, 0, stdout);
                }
                else {
                    freeCalComp(pcomp);
                    pcomp = NULL;
                }
            }
            else if (strcmp(argv[2], "t") == 0){
                status = readCalFile (stdin, &pcomp);
                if (status.code == OK){
                    status = calFilter (pcomp, OTODO, 0, 0, stdout);
                }
                else {
                    freeCalComp(pcomp);
                    pcomp = NULL;
                }
            }
            else {
                printMess(4);
            }
        }
        else if (argc == 5){
            if (strcmp(argv[2], "e") == 0 || strcmp(argv[2], "t") == 0){
                if (strcmp(argv[3],"from") == 0){
                    status = readCalFile (stdin, &pcomp);
                    if (status.code == OK){
                        if (strcmp(argv[4],"today") == 0){
                            time (&fromt);
                            from = localtime (&fromt);
                        }
                        else {
                            from = malloc (sizeof (struct tm));
                            assert (from != NULL);
                            date = getdate_r (argv[4], from);
                        }
                        if (strcmp(argv[4],"today") == 0 || date == 0){
                            from->tm_sec = 0;
                            from->tm_min = 0;
                            from->tm_hour = 0;
                            from->tm_isdst = -1;
                            fromt = mktime(from);
                            if (strcmp(argv[4],"today") == 0){
                                from = NULL;
                            }
                            if (strcmp(argv[2], "e") == 0){
                                status = calFilter (pcomp, OEVENT, fromt , 0, stdout);
                            }
                            else if (strcmp(argv[2], "t") == 0){
                                status = calFilter (pcomp, OTODO, fromt , 0, stdout);
                            }
                        }
                        else {
                            if (date > 0 && date < 6){
                                printf ("Problem with DATEMSK environment variable or template file\n");
                            }
                            else if (date > 6){
                                printf ("Date \"%s\" could not be interpreted\n",argv[4] );
                            }
                        }
                    }
                    else {
                        freeCalComp(pcomp);
                        pcomp = NULL;
                    }
                }
                else if (strcmp(argv[3],"to") == 0){
                    status = readCalFile (stdin, &pcomp);
                    if (status.code == OK){
                         if (strcmp(argv[4],"today") == 0){
                            time (&tot);
                            to = localtime (&tot);
                        }
                        else {
                            to = malloc (sizeof (struct tm));
                            assert (to != NULL);
                            date = getdate_r (argv[4], to);
                        }
                        if (strcmp(argv[4],"today") == 0 || date == 0){
                            to->tm_sec = 0;
                            to->tm_min = 59;
                            to->tm_hour = 23;
                            to->tm_isdst = -1;
                            tot = mktime(to);
                            if (strcmp(argv[4],"today") == 0){
                                to = NULL;
                            }
                            if (strcmp(argv[2], "e") == 0){
                                status = calFilter (pcomp, OEVENT, 0, tot, stdout);
                            }
                            else if (strcmp(argv[2], "t") == 0){
                                status = calFilter (pcomp, OTODO, 0, tot, stdout);
                            }
                        }
                        else {
                            if (date > 0 && date < 6){
                                printf ("Problem with DATEMSK environment variable or template file\n");
                            }
                            else if (date > 6){
                                printf ("Date \"%s\" could not be interpreted\n",argv[4] );
                            }
                        }
                    }
                    else {
                        freeCalComp(pcomp);
                        pcomp = NULL;
                    }
                }
                else {
                    printMess(5);
                }
            }
            else {
                printMess(4);
            }
        }
        else if (argc == 7){
            if (strcmp(argv[3], "from") == 0 && strcmp(argv[5], "to") == 0){
                if (strcmp(argv[2], "e") == 0 || strcmp(argv[2], "t") == 0){
                    status = readCalFile (stdin, &pcomp);
                    if (status.code == OK){
                        if (strcmp(argv[4], "today") == 0){
                            time (&fromt);
                            from = localtime (&fromt);
                        }
                        else {
                            from = malloc (sizeof (struct tm));
                            assert (from != NULL);
                            date = getdate_r (argv[4], from);
                        }
                        if (strcmp(argv[4],"today") == 0 || date == 0){
                            from->tm_sec = 0;
                            from->tm_min = 0;
                            from->tm_hour = 0;
                            from->tm_isdst = -1;
                            fromt = mktime(from);
                            if (strcmp(argv[4],"today") == 0){
                                from = NULL;
                            }
                            if (strcmp(argv[6], "today") == 0){
                                time (&tot);
                                to = localtime (&tot);
                            }
                            else {
                                to = malloc (sizeof (struct tm));
                                assert (to != NULL);
                                date = getdate_r (argv[6], to);
                            }
                            if (strcmp(argv[6], "today") == 0 || date == 0){
                                to->tm_sec = 0;
                                to->tm_min = 59;
                                to->tm_hour = 23;
                                to->tm_isdst = -1;
                                tot = mktime(to);
                                if (strcmp(argv[6],"today") == 0){
                                    to = NULL;
                                }
                                if (strcmp(argv[2], "e") == 0){
                                    status = calFilter (pcomp, OEVENT, fromt, tot, stdout);
                                }
                                else if (strcmp(argv[2], "t") == 0){
                                    status = calFilter (pcomp, OTODO, fromt, tot, stdout);
                                }
                            }
                            else {
                                if (date > 0 && date < 6){
                                    printf ("Problem with DATEMSK environment variable or template file\n");
                                }
                                else if (date > 6){
                                    printf ("Date \"%s\" could not be interpreted\n",argv[6] );
                                }
                            }
                        }
                        else {
                            if (date > 0 && date < 6){
                                printf ("Problem with DATEMSK environment variable or template file\n");
                            }
                            else if (date > 6){
                                printf ("Date \"%s\" could not be interpreted\n",argv[4] );
                            }
                        }
                    }
                    else {
                        freeCalComp(pcomp);
                        pcomp = NULL;
                    }
                }
                else {
                    printMess(4);
                }
            }
            else {
                printMess(5);
            }
        }
        else {
            printMess(6);
        }
    }
    else if (strcmp(argv[1], "-combine") == 0){
        if (argc == 3){
            fp = fopen (argv[2], "r");
            if (fp != NULL){
                status = readCalFile (stdin, &pcomp);
                if (status.code == OK){
                    status2 = readCalFile (fp, &pcomp2);
                    if (status2.code == OK){
                        status = calCombine (pcomp, pcomp2, stdout);
                        fclose (fp);
                    }
                    else {
                        freeCalComp(pcomp2);
                        pcomp2 = NULL;
                    }
                }
                else {
                    freeCalComp(pcomp);
                    pcomp = NULL;
                }
            }
            else {
                printMess(9);
            }
        }
        else {
            printMess(7);
        }
    }
    }
    else {
        printMess(8);
    }

    if (pcomp != NULL){
        freeCalComp(pcomp);
    }
    if (pcomp2 != NULL){
        freeCalComp(pcomp2);
    }
    if (from != NULL){
        free (from);
    }
    if (to != NULL){
        free (to);
    }

    if (status.code == OK && status2.code == OK){
        return EXIT_SUCCESS;
    }
    else {
        return EXIT_FAILURE;
    }
}

CalStatus calInfo( const CalComp *comp, int lines, FILE *const txtfile ){
    CalStatus stat;
    stat.code = OK;
    CalProp * prtemp = NULL;
    CalParam * patemp = NULL;
    char ** names = NULL;
    char * name = NULL;
    int ret = 0;
    int length = 0;
    int nstr = 0;
    bool match = false;
    int comps = 0;
    int event = 0;
    int todo = 0;
    int other = 0;
    int sub = 0;
    int prop = 0;
    time_t tempt;
    time_t firstt;
    time_t lastt;
    struct tm temp;
    struct tm first;
    struct tm last;
    int ndate = 0;
    char fstr[30];
    char lstr[30];

    prop = prop + comp->nprops;
    for (int i = 0; i < comp->ncomps; i++){
        prop = prop + comp->comp[i]->nprops;
        sub += comp->comp[i]->ncomps;
        if (strcmp(comp->comp[i]->name, "VEVENT") == 0){
            event++;
        }
        else if (strcmp(comp->comp[i]->name, "VTODO") == 0){
            todo++;
        }
        else {
            other++;
        }
        prtemp = comp->comp[i]->prop;
        while (prtemp != NULL){
            if (strcmp(prtemp->name, "ORGANIZER") == 0){
                patemp = prtemp->param;
                while (patemp != NULL){
                    if (strcmp(patemp->name, "CN") == 0){
                        length = strlen(patemp->value[0]);
                        name = realloc (name, sizeof (char) * (length+1));
                        assert (name != NULL);
                        strcpy(name,patemp->value[0]);
                        if (nstr == 0){
                            names = malloc (sizeof (char*));
                            assert (names != NULL);
                            names[0] = malloc (sizeof (char) * (length+1));
                            assert (names[0] != NULL);
                            strcpy (names[0], name);
                            nstr++;
                        }
                        else {
                            match = false;
                            for (int a = 0; a < nstr; a++){
                                if (strcmp (names[a],name) == 0){
                                    match = true;
                                }
                            }
                            if (match == false){
                                names = realloc (names, sizeof (char*)*(nstr+1));
                                assert (names != NULL);
                                names[nstr] = malloc (sizeof (char) * (length+1));
                                assert (names[nstr] != NULL);
                                strcpy (names[nstr], name);
                                nstr++;
                            }
                        }
                    }
                    patemp = patemp->next;
                }
            }
            if ((strcmp(prtemp->name,"COMPLETED") == 0 || strcmp(prtemp->name,"DTEND") == 0
               || strcmp(prtemp->name,"DUE") == 0 || strcmp(prtemp->name,"DTSTART") == 0
               || strcmp(prtemp->name,"CREATED") == 0 || strcmp(prtemp->name,"DTSTAMP") == 0
               || strcmp(prtemp->name,"LAST-MODIFIED") == 0)
               && strcmp(comp->comp[i]->name,"VTIMEZONE") != 0){
                strptime(prtemp->value, "%Y%m%dT%H%M%S", &temp);
                temp.tm_isdst = -1;
                tempt = mktime (&temp);
                if (ndate == 0){
                    memcpy (&first, &temp, sizeof(struct tm));
                    memcpy (&last, &temp, sizeof(struct tm));
                    memcpy (&firstt, &tempt, sizeof(time_t));
                    memcpy (&lastt, &tempt, sizeof(time_t));
                    ndate++;
                }
                else if (ndate > 0){
                    if (difftime(firstt, tempt) > 0){
                        memcpy (&first, &temp, sizeof(struct tm));
                        memcpy (&firstt, &tempt, sizeof(time_t));
                    }
                    if (difftime(tempt,lastt) > 0){
                        memcpy (&last, &temp, sizeof(struct tm));
                        memcpy (&lastt, &tempt, sizeof(time_t));
                    }
                }
            }
            prtemp = prtemp->next;
        }
        for (int j = 0; j < comp->comp[i]->ncomps; j++){
            prop = prop + comp->comp[i]->comp[j]->nprops;
            prtemp = comp->comp[i]->comp[j]->prop;
            while (prtemp != NULL){
                if ((strcmp(prtemp->name,"COMPLETED") == 0 || strcmp(prtemp->name,"DTEND") == 0
                   || strcmp(prtemp->name,"DUE") == 0 || strcmp(prtemp->name,"DTSTART") == 0
                   || strcmp(prtemp->name,"CREATED") == 0 || strcmp(prtemp->name,"DTSTAMP") == 0
                   || strcmp(prtemp->name,"LAST-MODIFIED") == 0)
                   && strcmp(comp->comp[i]->comp[j]->name,"VTIMEZONE") != 0){
                    strptime(prtemp->value, "%Y%m%dT%H%M%S", &temp);
                    temp.tm_isdst = -1;
                    tempt = mktime (&temp);
                    if (ndate == 0){
                        memcpy (&first, &temp, sizeof(struct tm));
                        memcpy (&last, &temp, sizeof(struct tm));
                        memcpy (&firstt, &tempt, sizeof(time_t));
                        memcpy (&lastt, &tempt, sizeof(time_t));
                        ndate++;
                    }
                    else if (ndate > 0){
                        if (difftime(firstt, tempt) > 0){
                            memcpy (&first, &temp, sizeof(struct tm));
                            memcpy (&firstt, &tempt, sizeof(time_t));
                        }
                        if (difftime(tempt,lastt) > 0){
                            memcpy (&last, &temp, sizeof(struct tm));
                            memcpy (&lastt, &tempt, sizeof(time_t));
                        }
                    }
                }
                prtemp = prtemp->next;
            }
        }
    }
    qsort (names, nstr, sizeof(char*), compareStr);
    comps = event + todo + other;

    if (lines == 1){
        ret = fprintf (txtfile, "%d line\n", lines);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    else {
        ret = fprintf (txtfile, "%d lines\n", lines);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    if (comps == 1){
        ret = fprintf (txtfile, "%d component: ", comps);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    else {
        ret = fprintf (txtfile, "%d components: ", comps);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    if (event == 1){
        ret = fprintf (txtfile, "%d event, ", event);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    else {
        ret = fprintf (txtfile, "%d events, ", event);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    if (todo == 1){
        ret = fprintf (txtfile, "%d todo, ", todo);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    else {
        ret = fprintf (txtfile, "%d todos, ", todo);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    if (other == 1){
        ret = fprintf (txtfile, "%d other\n", other);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    else {
        ret = fprintf (txtfile, "%d others\n", other);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    if (sub == 1){
        ret = fprintf (txtfile, "%d subcomponent\n", sub);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    else {
        ret = fprintf (txtfile, "%d subcomponents\n", sub);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    if (prop == 1){
        ret = fprintf (txtfile, "%d property\n", prop);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    else {
        ret = fprintf (txtfile, "%d properties\n", prop);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }

    if (ndate == 0){
        ret = fprintf(txtfile, "No dates\n");
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    else if (ndate > 0){
        strftime (fstr, 30, "%Y-%b-%d", &first);
        strftime (lstr, 30, "%Y-%b-%d", &last);
        ret = fprintf (txtfile, "From %s to %s\n", fstr, lstr);
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    if (nstr == 0){
        ret = fprintf (txtfile, "No organizers\n");
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
    }
    else {
        ret = fprintf (txtfile, "Organizers:\n");
        if (ret < 0){
            for (int a = 0; a < nstr; a++){
                free (names[a]);
            }
            free (names);
            free (name);
            stat.code = IOERR;
            return stat;
        }
        for (int i = 0; i < nstr; i++){
            ret = fprintf(txtfile, "%s\n", names[i]);
            if (ret < 0){
                for (int a = 0; a < nstr; a++){
                    free (names[a]);
                }
                free (names);
                free (name);
                stat.code = IOERR;
                return stat;
            }
        }
    }

    free (name);
    for (int i = 0; i < nstr; i++){
        free (names[i]);
    }
    free (names);

    return stat;
}

CalStatus calExtract( const CalComp *comp, CalOpt kind, FILE *const txtfile ){
    CalStatus stat;
    stat.code = OK;
    CalProp * prtemp = NULL;
    struct tm temp;
//    time_t tempt;
    char dtstr[30];
    int lengtht = 0;
    int lengths = 0;
    bool found = false;
    char * summ = NULL;
    char * event = NULL;
    char ** events = NULL;
    int neve = 0;
    char ** props = NULL;
    char * prop = NULL;
    int nprop = 0;
    int ret = 0;

    if (kind == OEVENT){
        for (int i = 0; i < comp->ncomps; i++){
            if (strcmp(comp->comp[i]->name, "VEVENT") == 0){
                prtemp = comp->comp[i]->prop;
                while (prtemp != NULL){
                    if (strcmp (prtemp->name, "DTSTART") == 0){
                        strptime(prtemp->value, "%Y%m%dT%H%M%S", &temp);
                        temp.tm_isdst = -1;
//                        tempt = mktime (&temp);
                        strftime (dtstr, 30, "%Y-%b-%d %l:%M %p", &temp);
                        lengtht = strlen(dtstr) + 1;
                    }
                    else if (strcmp (prtemp->name, "SUMMARY") == 0){
                        found = true;
                        lengths = strlen (prtemp->value) + 1;
                        summ = realloc (summ, sizeof (char) * lengths);
                        assert (summ != NULL);
                        strcpy (summ, prtemp->value);
                    }
                    prtemp = prtemp->next;
                }
                if (found == true){
                    event = malloc (sizeof (char) * (lengtht + lengths + 5));
                    assert (event != NULL);
                    strcpy (event, dtstr);
                    strcat (event, ": ");
                    strcat (event, summ);
                    found = false;
                }
                else {
                    event = malloc (sizeof (char) * (lengtht + 12));
                    assert (event != NULL);
                    strcpy (event, dtstr);
                    strcat (event, ": (na)");
                }
                events = realloc (events, sizeof (char*) * (neve+1));
                assert (events != NULL);
                events[neve] = event;
                neve++;
            }
        }
        qsort (events, neve, sizeof (char*), compareDate);
        for (int j = 0; j < neve; j++){
            ret = fprintf (txtfile, "%s\n", events[j]);
            if (ret < 0){
                if (summ != NULL){
                    free (summ);
                }
                if (events != NULL){
                    for (int a = 0; a < neve; a++){
                        free (events[a]);
                    }
                    free (events);
                }
                stat.code = IOERR;
                return stat;
            }
        }
        free (summ);
        for (int j = 0; j < neve; j++){
            free (events[j]);
        }
        free (events);
    }
    else if (kind == OPROP){
        for (int i = 0; i < comp->ncomps; i++){
            prtemp = comp->comp[i]->prop;
            while (prtemp != NULL){
                if (prtemp->name[0] == 'X' && prtemp->name[1] == '-'){
                    lengths = strlen (prtemp->name);
                    if (nprop == 0){
                        prop = malloc (sizeof (char) * (lengths+1));
                        assert (prop != NULL);
                        strcpy (prop, prtemp->name);
                        props = malloc (sizeof (char*) * (nprop+1));
                        assert (props != NULL);
                        props[nprop] = prop;
                        nprop++;
                    }
                    else {
                        found = false;
                        for (int k = 0; k < nprop; k++){
                            if (strcmp(props[k],prtemp->name) == 0){
                                found = true;
                            }
                        }
                        if (found == false){
                            prop = malloc (sizeof (char) * (lengths+1));
                            assert (prop != NULL);
                            strcpy (prop, prtemp->name);
                            props = realloc (props, sizeof (char*) * (nprop+1));
                            assert (props != NULL);
                            props[nprop] = prop;
                            nprop++;
                        }
                    }
                }
                prtemp = prtemp->next;
            }
            for (int j = 0; j < comp->comp[i]->ncomps; j++){
                prtemp = comp->comp[i]->comp[j]->prop;
                while (prtemp != NULL){
                    if (prtemp->name[0] == 'X' && prtemp->name[1] == '-'){
                        lengths = strlen (prtemp->name);
                        if (nprop == 0){
                            prop = malloc (sizeof (char) * (lengths+1));
                            assert (prop != NULL);
                            strcpy (prop, prtemp->name);
                            props = malloc (sizeof (char*) * (nprop+1));
                            assert (props != NULL);
                            props[nprop] = prop;
                            nprop++;
                        }
                        else {
                            found = false;
                            for (int k = 0; k < nprop; k++){
                                if (strcmp(props[k],prtemp->name) == 0){
                                    found = true;
                                }
                            }
                            if (found == false){
                                prop = malloc (sizeof (char) * (lengths+1));
                                assert (prop != NULL);
                                strcpy (prop, prtemp->name);
                                props = realloc (props, sizeof (char*) * (nprop+1));
                                assert (props != NULL);
                                props[nprop] = prop;
                                nprop++;
                            }
                        }
                    }
                    prtemp = prtemp->next;
                }
            }
        }
        qsort (props, nprop, sizeof (char*), compareStr);
        for(int a = 0; a < nprop; a++){
            ret = fprintf (txtfile, "%s\n", props[a]);
            if (ret < 0){
                if (props != NULL){
                    for (int a = 0; a < nprop; a++){
                        free (props[a]);
                    }
                    free (props);
                }
                stat.code = IOERR;
                return stat;
            }
        }
        for (int a = 0; a < nprop; a++){
            free (props[a]);
        }
        free (props);
    }

    return stat;
}

CalStatus calFilter( const CalComp *comp, CalOpt content, time_t datefrom, time_t dateto, FILE *const icsfile ){
    CalStatus stat;
    stat.code = OK;
    time_t curt;
    struct tm cur;
    bool inrange = false;
    int compsize = 0;
    CalProp * prtemp = NULL;
    CalComp * temp;
    int num = 0;
    compsize = sizeof (CalComp) + (sizeof (CalComp*) * comp->ncomps);

    temp = malloc (compsize);
    assert (temp != NULL);
    memcpy (temp, comp, compsize);

    if (content == OEVENT){
        while (num < temp->ncomps){
            if (strcmp(temp->comp[num]->name, "VEVENT") != 0){
                temp->ncomps--;
                for (int j = num; j < temp->ncomps; j++){
                    temp->comp[j] = temp->comp[j+1];
                }
            }
            else if (strcmp(temp->comp[num]->name, "VEVENT") == 0){
                inrange = false;
                prtemp = temp->comp[num]->prop;
                while (prtemp != NULL){
                    if (strcmp(prtemp->name,"COMPLETED") == 0 || strcmp(prtemp->name,"DTEND") == 0
                       || strcmp(prtemp->name,"DUE") == 0 || strcmp(prtemp->name,"DTSTART") == 0){
                        strptime(prtemp->value, "%Y%m%dT%H%M%S", &cur);
                        cur.tm_isdst = -1;
                        curt = mktime (&cur);
                        if (datefrom == 0 && dateto == 0){
                            inrange = true;
                            break;
                        }
                        else if (datefrom != 0 && dateto == 0){
                            if (difftime (curt,datefrom) >= 0){
                                inrange = true;
                                break;
                            }
                        }
                        else if (datefrom == 0 && dateto != 0){
                            if (difftime (dateto,curt) >= 0){
                                inrange = true;
                                break;
                            }
                        }
                        else {
                            if (difftime (curt,datefrom) >= 0 && difftime (dateto,curt) >= 0){
                                inrange = true;
                                break;
                            }
                        }
                    }
                    prtemp = prtemp->next;
                }
                if (inrange == false){
                    for (int k = 0; k < temp->comp[num]->ncomps; k++){
                        prtemp = temp->comp[num]->comp[k]->prop;
                        while (prtemp != NULL){
                            if (strcmp(prtemp->name,"COMPLETED") == 0 || strcmp(prtemp->name,"DTEND") == 0
                               || strcmp(prtemp->name,"DUE") == 0 || strcmp(prtemp->name,"DTSTART") == 0){
                                strptime(prtemp->value, "%Y%m%dT%H%M%S", &cur);
                                cur.tm_isdst = -1;
                                curt = mktime (&cur);
                                if (datefrom == 0 && dateto == 0){
                                    inrange = true;
                                    break;
                                }
                                else if (datefrom != 0 && dateto == 0){
                                    if (difftime (curt,datefrom) >= 0){
                                        inrange = true;
                                        break;
                                    }
                                }
                                else if (datefrom == 0 && dateto != 0){
                                    if (difftime (dateto,curt) >= 0){
                                        inrange = true;
                                        break;
                                    }
                                }
                                else {
                                    if (difftime (curt,datefrom) >= 0 && difftime (dateto,curt) >= 0){
                                        inrange = true;
                                        break;
                                    }
                                }
                            }
                            prtemp = prtemp->next;
                        }
                        if (inrange == true){
                            break;
                        }
                    }
                }
                if (inrange == false){
                    temp->ncomps--;
                    for (int j = num; j < temp->ncomps; j++){
                        temp->comp[j] = temp->comp[j+1];
                    }
                }
                else if (inrange == true){
                    num++;
                }
            }
            else {
                num++;
            }
        }
    }
    else if (content == OTODO){
        while (num < temp->ncomps){
            if (strcmp(temp->comp[num]->name, "VTODO") != 0){
                temp->ncomps--;
                for (int j = num; j < temp->ncomps; j++){
                    temp->comp[j] = temp->comp[j+1];
                }
            }
            else if (strcmp(temp->comp[num]->name, "VEVENT") == 0){
                inrange = false;
                prtemp = temp->comp[num]->prop;
                while (prtemp != NULL){
                    if (strcmp(prtemp->name,"COMPLETED") == 0 || strcmp(prtemp->name,"DTEND") == 0
                       || strcmp(prtemp->name,"DUE") == 0 || strcmp(prtemp->name,"DTSTART") == 0){
                        strptime(prtemp->value, "%Y%m%dT%H%M%S", &cur);
                        cur.tm_isdst = -1;
                        curt = mktime (&cur);
                        if (datefrom == 0 && dateto == 0){
                            inrange = true;
                            break;
                        }
                        else if (datefrom != 0 && dateto == 0){
                            if (difftime (curt,datefrom) >= 0){
                                inrange = true;
                                break;
                            }
                        }
                        else if (datefrom == 0 && dateto != 0){
                            if (difftime (dateto,curt) >= 0){
                                inrange = true;
                                break;
                            }
                        }
                        else {
                            if (difftime (curt,datefrom) >= 0 && difftime (dateto,curt) >= 0){
                                inrange = true;
                                break;
                            }
                        }
                    }
                    prtemp = prtemp->next;
                }
                if (inrange == false){
                    for (int k = 0; k < temp->comp[num]->ncomps; k++){
                        prtemp = temp->comp[num]->comp[k]->prop;
                        while (prtemp != NULL){
                            if (strcmp(prtemp->name,"COMPLETED") == 0 || strcmp(prtemp->name,"DTEND") == 0
                               || strcmp(prtemp->name,"DUE") == 0 || strcmp(prtemp->name,"DTSTART") == 0){
                                strptime(prtemp->value, "%Y%m%dT%H%M%S", &cur);
                                cur.tm_isdst = -1;
                                curt = mktime (&cur);
                                if (datefrom == 0 && dateto == 0){
                                    inrange = true;
                                    break;
                                }
                                else if (datefrom != 0 && dateto == 0){
                                    if (difftime (curt,datefrom) >= 0){
                                        inrange = true;
                                        break;
                                    }
                                }
                                else if (datefrom == 0 && dateto != 0){
                                    if (difftime (dateto,curt) >= 0){
                                        inrange = true;
                                        break;
                                    }
                                }
                                else {
                                    if (difftime (curt,datefrom) >= 0 && difftime (dateto,curt) >= 0){
                                        inrange = true;
                                        break;
                                    }
                                }
                            }
                            prtemp = prtemp->next;
                        }
                        if (inrange == true){
                            break;
                        }
                    }
                }
                if (inrange == false){
                    temp->ncomps--;
                    for (int j = num; j < temp->ncomps; j++){
                        temp->comp[j] = temp->comp[j+1];
                    }
                }
                else if (inrange == true){
                    num++;
                }
            }
            else {
                num++;
            }
        }
    }
    if (temp->ncomps > 0){
        stat = writeCalComp (icsfile, temp);
    }
    else {
        stat.code = NOCAL;
    }
    free (temp);
    return stat;
}

CalStatus calCombine( const CalComp *comp1, const CalComp *comp2, FILE *const icsfile ){
    CalStatus stat;
    stat.code = OK;
    int compsize = 0;
    CalProp * prcomp = NULL;
    CalProp * prcur = NULL;
    CalProp * prtemp = NULL;
    CalComp * temp;
    int index = 1;
    int pos = 0;
    CalProp ** props = NULL;

    compsize = sizeof (CalComp) + (sizeof (CalComp*) * comp1->ncomps) +
              (sizeof (CalComp*) * comp2->ncomps);

    temp = malloc (compsize);
    assert (temp != NULL);
    memcpy (temp, comp1, compsize - (sizeof (CalComp*) * comp2->ncomps));

    for (int i = 0; i < comp2->ncomps; i++){
        temp->comp[temp->ncomps] = comp2->comp[i];
        temp->ncomps++;
    }

    props = malloc (sizeof (CalProp*) * comp2->nprops);
    assert (props != NULL);

    prcomp = comp2->prop;
    while (pos < comp2->nprops){
        props[pos] = prcomp;
        prcomp = prcomp->next;
        pos++;
    }

    prtemp = temp->prop;
    while (prtemp->next != NULL){
        prtemp = prtemp->next;
    }

    prtemp->next = comp2->prop;
    temp->nprops = temp->nprops + comp2->nprops;

    prcur = prtemp;
    while (prcur->next != NULL){
        if (strcmp(prcur->next->name, "VERSION") == 0 || strcmp (prcur->next->name, "PRODID") == 0){
            prcur = prcur->next;
            temp->nprops--;
        }
        else {
            prcur = prcur->next;
            prtemp->next = prcur;
            prtemp = prtemp->next;
        }
    }
    prtemp->next = NULL;

    stat = writeCalComp (icsfile, temp);

    prcomp = comp1->prop;
    while (index < comp1->nprops){
        prcomp = prcomp->next;
        index++;
    }
    prcomp->next = NULL;

    pos = 1;
    prcomp = comp2->prop;
    while (pos < comp2->nprops){
        prcomp->next = props[pos];
        prcomp = prcomp->next;
        pos++;
    }
    prcomp->next = NULL;

    free (props);
    free (temp);

    return stat;
}

int compareStr (const void * one, const void * two){
    const char * pone = *(const char**)one;
    const char * ptwo = *(const char**)two;
    return strcmp(pone,ptwo);
}

int compareDate (const void * one, const void * two){
    struct tm done;
    struct tm dtwo;
    const char * fir = *(const char**)one;
    const char * sec = *(const char**)two;

    strptime (fir, "%Y-%b-%d %I:%M %p", &done);
    strptime (sec, "%Y-%b-%d %I:%M %p", &dtwo);

    if (done.tm_year - dtwo.tm_year != 0){
        return done.tm_year - dtwo.tm_year;
    }
    if (done.tm_mon - dtwo.tm_mon != 0){
        return done.tm_mon - dtwo.tm_mon;
    }
    if (done.tm_mday - dtwo.tm_mday != 0){
        return done.tm_mday - dtwo.tm_mday;
    }
    if (done.tm_hour - dtwo.tm_hour != 0){
        return done.tm_hour - dtwo.tm_hour;
    }
    if (done.tm_min - dtwo.tm_min != 0){
        return done.tm_min - dtwo.tm_min;
    }
    return 0;

}

void printMess (int error){
    switch(error){

        case 1:
            fprintf (stderr, "Fatal error: Incorrect syntax for 'info'\n"
                "Proper syntax: \"./caltool -info\"\n");
            break;

        case 2:
            fprintf (stderr, "Fatal error: Incorrect type option for 'extract'\n"
                "Options allowed: 'e' or 'x'\n"
                "Legend: '()'- required, '[]'- optional, '/'- OR\n"
                "Proper syntax:\n"
                "\"./caltool -extract (e/x)\"\n");
        break;

        case 3:
            fprintf (stderr, "Fatal error: Incorrect syntax for 'extract'\n"
                "Options allowed: 'e' or 'x'\n"
                "Legend: '()'- required, '[]'-optional, '/'- OR\n"
                "Proper syntax:\n"
                "\"./caltool -extract (e/x)\"\n");
            break;

        case 4:
            fprintf (stderr, "Fatal error: Incorrect type option for 'filter'\n"
                "Options allowed: 'e' or 't'\n"
                "Legend: '()'- required, '[]'-optional, '/'- OR\n"
                "Proper syntax:\n"
                "\"./caltool -filter (e/t) [from (\"date\")] [to (\"date\")]\"\n");
            break;

        case 5:
            fprintf (stderr, "Fatal error: Incorrect date input option for 'filter'\n"
                "Options allowed: 'from' or 'to'\n"
                "Legend: '()'- required, '[]'-optional, '/'- OR\n"
                "Proper syntax:\n"
                "\"./caltool -filter (e/t) [from (\"date\")] [to (\"date\")]\"\n");
            break;

        case 6:
            fprintf (stderr, "Fatal error: Incorrect syntax for 'filter'\n"
                "Legend: '()'- required, '[]'-optional, '/'- OR\n"
                "Proper syntax:\n"
                "\"./caltool -filter (e/t) [from (\"date\")] [to (\"date\")]\"\n");
            break;

        case 7:
            fprintf (stderr, "Fatal error: Incorrect syntax for 'combine'\n"
                "Legend: '()'- required, '[]'-optional, '/'- OR\n"
                "Proper syntax:\n"
                "\"./caltool -combine (\"ics filename\")\"\n");
            break;

        case 8:
            fprintf (stderr, "Fatal error: Incorrect syntax for 'caltool'\n"
                "Legend: '()'- required, '[]'-optional, '/'- OR\n"
                "Proper syntaxes:\n"
                "\"./caltool -info\"\n"
                "\"./caltool -extract (e/x)\"\n"
                "\"./caltool -filter (e/t) [from (\"date\")] [to (\"date\")]\"\n"
                "\"./caltool -combine (\"ics filename\")\"\n");
            break;

        case 9:
            fprintf (stderr, "Fatal error: File cannot be read\n");
            break;
    }


}
