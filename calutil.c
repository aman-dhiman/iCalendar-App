/***********
calutil.c

Functions for parsing ics files and creating a Calendar structure
***********/

#include "calutil.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

//This function was created to print the calendar just as
//writeCalComp during assignment 1. Similar function.
void printCalendar (CalComp * const pcomp, FILE * txtfile);
//PyObject *Cal_writeFile( PyObject *self, PyObject *args );
//PyObject *Cal_freeFile( PyObject *self, PyObject *args );

CalStatus readCalFile (FILE *const ics, CalComp ** const pcomp){
    readCalLine (NULL,NULL);
    CalProp * ptemp = NULL;
    CalComp * ctemp = NULL;
    int nver = 0;
    int nprod = 0;
    bool vwrong = false;
    bool vcomp = false;
    CalStatus status;
    status.code = OK;
    char * buff = NULL;
    *pcomp = malloc (sizeof (CalComp) + sizeof (CalComp*));
    assert (*pcomp != NULL);
    pcomp[0]->name = NULL;
    pcomp[0]->nprops = 0;
    pcomp[0]->prop = NULL;
    pcomp[0]->ncomps = 0;
    pcomp[0]->comp[0] = NULL;
    CalStatus stat;
    stat.code = OK;
    stat.linefrom = 0;
    stat.lineto = 0;
    stat = readCalComp (ics,pcomp);
    if (stat.code == OK){
    ptemp = pcomp[0]->prop;
    while (ptemp != NULL){
        if (strcmp (ptemp->name, "VERSION") == 0){
            nver++;
            if (strcmp (ptemp->value, VCAL_VER) != 0){
                vwrong = true;
            }
        }
        if (strcmp (ptemp->name, "PRODID") == 0){
            nprod++;
        }
        ptemp = ptemp->next;
    }
    for (int i = 0; i < pcomp[0]->ncomps; i++){
        ctemp = pcomp[0]->comp[i];
        if (ctemp->name[0] == 'V'){
            vcomp = true;
        }
    }
    if (nver != 1 || vwrong == true){
        stat.code = BADVER;
    }
    else if (nprod != 1){
        stat.code = NOPROD;
    }
    else if (pcomp[0]->ncomps == 0 || vcomp == false){
        stat.code = NOCAL;
    }
    else {
        status = readCalLine (ics, &buff);
        if (buff != NULL){
            stat.code = AFTEND;
            free (buff);
            buff = NULL;
        }
        if (status.code == OK){
            status.code = OK;
        }
    }
    }
    if (stat.code != OK){
        freeCalComp (*pcomp);
        *pcomp = NULL;
    }
    return stat;
}

CalStatus readCalComp( FILE *const ics, CalComp **const pcomp ){
    CalStatus stat;
    int index = 0;
    static int level = 0;
    char * pbuff = NULL;
    CalProp * temp = NULL;
    CalProp * cur = NULL;
    CalProp * prop = NULL;
    do {
    stat = readCalLine (ics, &pbuff);
    prop = malloc (sizeof (CalProp));
    assert (prop != NULL);

    if (pbuff == NULL && level == 0){
        if (stat.code == OK){
            stat.code = NOCAL;
        }
        if (prop != NULL){
            free (prop);
            prop = NULL;
        }
        return stat;
    }
    else if (pbuff == NULL && level > 0){
        if (stat.code == OK){
            stat.code = BEGEND;
        }
        if (prop != NULL){
            free (prop);
            prop = NULL;
        }
        return stat;
    }

    stat.code = parseCalProp (pbuff, prop);
    if (stat.code != OK){
        if (prop != NULL){
            free (prop);
            prop = NULL;
        }
        if (pbuff != NULL){
            free (pbuff);
            pbuff = NULL;
        }
        return stat;
    }

    if (level <= 3){
        if (level == 0){
            if ((strcmp (prop->name, "BEGIN") == 0) && (strcmp (prop->value, "VCALENDAR") == 0)){
                pcomp[0]->name = malloc (sizeof (char) * 80);
                assert (pcomp[0]->name != NULL);
                strcpy (pcomp[0]->name, prop->value);
                level = 1;
                free (prop->name);
                free (prop->value);
                free (prop);
                prop = NULL;
            }
            else {
                free (prop->name);
                free (prop->value);
                free (prop);
                prop = NULL;
                stat.code = NOCAL;
                if (prop != NULL){
                    free (prop);
                    prop = NULL;
                }
                if (pbuff != NULL){
                    free (pbuff);
                    pbuff = NULL;
                }
                return stat;
            }
        }
    }
        if (pbuff != NULL && prop != NULL){

        for (int i = 0; i < strlen(prop->name); i++){
            prop->name[i] = toupper(prop->name[i]);
        }
        if (strcmp (prop->name, "BEGIN") == 0){
                *pcomp = realloc (*pcomp, (sizeof (CalComp))+(sizeof (CalComp*)*(pcomp[0]->ncomps+1)));
                assert (*pcomp != NULL);
                pcomp[0]->comp[index] = malloc (sizeof (CalComp) + sizeof (CalComp*));
                assert (pcomp[0]->comp[index] != NULL);
                char name[80];
                strcpy (name, prop->value);
                int length = strlen(name);
                for (int i = 0; i < length; i++){
                    name[i] = toupper(name[i]);
                }
                pcomp[0]->comp[index]->name = malloc ((sizeof (char) * length) + 1);
                assert (pcomp[0]->comp[index]->name != NULL);
                strcpy (pcomp[0]->comp[index]->name, name);
                pcomp[0]->comp[index]->nprops = 0;
                pcomp[0]->comp[index]->prop = NULL;
                pcomp[0]->comp[index]->ncomps = 0;
                pcomp[0]->comp[index]->comp[0] = NULL;
                pcomp[0]->ncomps++;
                level++;
                index++;
                free (prop->name);
                free (prop->value);
                free (prop);
                prop = NULL;
            if (level <= 3){
                stat = readCalComp (ics, &(pcomp[0]->comp[index-1]));
                prop = NULL;
            }
            else if (level > 3){
                stat.code = SUBCOM;
                if (prop != NULL){
                    free (prop);
                    prop = NULL;
                }
                if (pbuff != NULL){
                    free (pbuff);
                    pbuff = NULL;
                }
                return stat;
            }
        }
        if (prop != NULL){
        for (int i = 0; i < strlen(prop->name); i++){
            prop->name[i] = toupper(prop->name[i]);
        }
        if (strcmp (prop->name, "END") == 0){
            char endName[80];
            strcpy (endName, prop->value);
            for (int i = 0; i < strlen (endName); i++){
                endName[i] = toupper(endName[i]);
            }
            free (prop->name);
            free (prop->value);
            free (prop);
            prop = NULL;
            free (pbuff);
            pbuff = NULL;
            level--;
            if (pcomp[0]->ncomps == 0 && pcomp[0]->nprops == 0){
                stat.code = NODATA;
                return stat;
            }
            if (strcmp (pcomp[0]->name, endName) != 0){
                stat.code = BEGEND;
                return stat;
            }
            stat.code = OK;
            return stat;
        }
        }
        }
        if (pbuff != NULL && prop != NULL){
            if (pcomp[0]->prop == NULL){
                pcomp[0]->prop = prop;
                 pcomp[0]->nprops++;
            }
            else {
                temp = pcomp[0]->prop;
                do {
                    cur = temp;
                    temp = cur->next;
                } while (temp != NULL);
                cur->next = prop;
                pcomp[0]->nprops++;
            }
        }
    }while (stat.code == OK && pbuff != NULL);

    if (pbuff != NULL){
        free (pbuff);
        pbuff = NULL;
    }
    return stat;
}

CalStatus readCalLine( FILE *const ics, char **const pbuff ){
    CalStatus stat;
    static char next = '\0';
    char nstr[2] = "";
    nstr[0] = next;
    nstr[1] = '\0';
    int crnl = 0;
    char str[100];
    int length;
    int nspace = 0;
    static int to = 0;
    static int from = 0;
    if (ics == NULL){
        next = '\0';
        stat.code = OK;
        stat.linefrom = 0;
        stat.lineto = 0;
        to = 0;
        from = 0;

    }
    else {
        if (*pbuff == NULL){
            *pbuff = malloc (sizeof (char) * 100);
            assert (*pbuff != NULL);
        }
        if (fgets (str, 100, ics) == NULL){
            free (*pbuff);
            *pbuff = NULL;
        }
        else {
            from = to;
            from++;
            do {
                nspace = 0;
                length = strlen(str);
                for (int i = 0; i < length; i++){
                    if (!isspace(str[i])){
                        nspace++;
                    }
                }
                if (nspace > 0){
                    break;
                }
                to++;
            }
            while (fgets (str, 100, ics) != NULL && nspace == 0);
            if (str[length - 1] == '\n' && str[length-2]  == '\r'){
                str[length-2] = '\0';
                str[length-1] = ' ';
                crnl = 1;
            }
            if (next != EOF && next != '\0'){
                strcpy (*pbuff, nstr);
                strcat (*pbuff, str);
            }
            else {
                strcpy (*pbuff, str);
            }
            if (crnl == 1){
                to++;
            }
            else {
                stat.code = NOCRNL;
                if (*pbuff != NULL){
                    free (*pbuff);
                    *pbuff = NULL;
                }
                return stat;
            }
            crnl = 0;
            next = fgetc(ics);
            nspace = 0;
            while (next != EOF && isspace(next)){
                fgets (str, 100, ics);
                if (next == ' '){
                    length = strlen (str);
                    for (int i = 0; i < length; i++){
                        if (!isspace(str[i])){
                            nspace++;
                        }
                    }
                    if (nspace > 0){
                        if (str[length - 1] == '\n' && str[length-2]  == '\r'){
                            str[length-2] = '\0';
                            str[length-1] = ' ';
                            crnl = 1;
                        }
                        if (crnl == 0){
                            stat.code = NOCRNL;
                            if (*pbuff != NULL){
                                free (*pbuff);
                                *pbuff = NULL;
                            }
                            return stat;
                        }
                        crnl = 0;
                        *pbuff = realloc (*pbuff, sizeof (char) * (strlen(*pbuff) + length + 1));
                        assert (*pbuff != NULL);
                        strcat (*pbuff, str);
                    }
                    nspace = 0;
                }
                next = fgetc (ics);
                to++;
            }
        }
        stat.lineto = to;
        stat.linefrom = from;
        stat.code = OK;
    }
    return stat;
}

CalError parseCalProp( char *const buff, CalProp *const prop ){
    int semi = 0;
    int colon = 0;
    int equal = 0;
    int comma = 0;
    int dquo = 0;
    bool inquo = false;
    bool metcolon = false;
    bool spaces = false;
    int done = 0;
    bool pon = false;
    bool von = false;
    bool son = false;
    bool eon = false;
    bool con = false;
    bool qon = false;
    char * pvalue = NULL;
    int qindex = 0;
    int sindex = 0;
    int vindex = 0;
    char * str = malloc (sizeof (char) * strlen(buff)+1);
    assert (str != NULL);
    char * token;
    CalParam ** temp = NULL;
    CalParam * cur = NULL;
    prop->name = malloc (sizeof (char) * 82);
    assert (prop->name != NULL);
    prop->value = malloc (sizeof (char) * 82);
    assert (prop->value != NULL);
    prop->nparams = 0;
    prop->param = NULL;
    prop->next = NULL;
    if (buff != NULL){
    strcpy (str, buff);
    for (int i = 0; i < strlen(buff); i++){
        if (buff[i] == '"' && metcolon == false){
            if (dquo%2 == 0) qon = true;
            else qon = false;
            dquo++;
        }
        if (buff[i] == ':' && metcolon == false && qon == false){
            metcolon = true;
        }
    }
    if (done == 0) done = 1;
    metcolon = false;
    int nquo = (dquo/2)+1;
    int count [nquo][3];
    for (int i = 0; i < nquo; i++){
        for (int j = 0; j < 3; j++){
            count[i][j] = 0;
        }
    }

    for (int i = 0; i < strlen(buff); i++){
        if (buff[i] == ';'){
            if (inquo == false && metcolon == false){
                count [qindex][1]++;
                semi++;
            }
        }
        if (buff[i] == '='){
            if (inquo == false && metcolon == false){
                equal++;
            }
        }
        if (buff[i] == ','){
            if (inquo == false && metcolon == false){
                count [qindex][0]++;
                comma++;
            }
        }
        if (buff[i] == '"'){
            if (inquo == false && metcolon == false){
                inquo = true;
            }
            else if (inquo == true && metcolon == false){
                inquo = false;
                qindex++;
            }
        }
        if (buff[i] == ':'){
            if (inquo == false && metcolon == false){
                count [qindex][2]++;
                colon++;
                metcolon = true;
            }
        }
    }
    if (colon != 1){
        if (str != NULL){
            free (str);
        }
        if (prop->name != NULL){
            free (prop->name);
        }
        if (prop->value != NULL){
            free (prop->value);
        }
        return SYNTAX;
    }
    if (semi != equal){
        if (str != NULL){
            free (str);
        }
        if (prop->name != NULL){
            free (prop->name);
        }
        if (prop->value != NULL){
            free (prop->value);
        }
        return SYNTAX;
    }
    if (dquo % 2 != 0){
        if (str != NULL){
            free (str);
        }
        if (prop->name != NULL){
            free (prop->name);
        }
        if (prop->value != NULL){
            free (prop->value);
        }
        return SYNTAX;
    }

    metcolon = false;
    inquo = false;
    int nparam [semi];
    for (int i = 0; i < semi; i++){
        nparam[i] = 1;
    }
    for (int i = 0; i < strlen(buff); i++){
        if (buff[i] == ';'){
            if (inquo == false && metcolon == false){
                sindex++;
            }
        }
        if (buff[i] == ','){
            if (inquo == false && metcolon == false){
                nparam[sindex-1]++;
            }
        }
        if (buff[i] == '"'){
            if (inquo == false && metcolon == false){
                inquo = true;
            }
            else if (inquo == true && metcolon == false){
                inquo = false;
            }
        }
        if (buff[i] == ':'){
            metcolon = true;
        }
    }

    if (semi == 0 && colon == 1){
        if (str[0] == ':'){
            if (str != NULL){
                free (str);
            }
            if (prop->name != NULL){
                free (prop->name);
            }
            if (prop->value != NULL){
                free (prop->value);
            }
            return SYNTAX;
        }
        token = strtok (str, ":");
        if (token != NULL){
        for (int i = 0; i < strlen(token); i++){
            token[i] = toupper(token[i]);
            if (token[i] == ' '){
                spaces = true;
            }
        }
        if (spaces == true){
            if (str != NULL){
                free (str);
            }
            if (prop->name != NULL){
                free (prop->name);
            }
            if (prop->value != NULL){
                free (prop->value);
            }
            return SYNTAX;
        }
        prop->name = realloc (prop->name, sizeof (char) * (strlen(token)+1));
        assert (prop->name != NULL);
        strcpy (prop->name, token);
        }
        token = strtok (NULL, "");
        if (token != NULL){
        prop->value = realloc (prop->value, sizeof (char) * (strlen(token)+1));
        assert (prop->value != NULL);
        strcpy (prop->value, token);
        }
        if ((token = strtok (NULL, ":")) == NULL) done = 1;
    }
    spaces = false;
    pvalue = malloc (sizeof (char)*2);
    assert (pvalue != NULL);
    strcpy(pvalue, "");
    int nq = 0;
    inquo = false;
    eon = true;
    con = false;
    sindex = 0;
    vindex = 0;
    str = realloc (str, sizeof (char) * (strlen(buff)+1));
    assert (str != NULL);
    if (semi > 0 && colon == 1){
    strcpy (str, buff);
    if (str[0] == ';'){
        if (str != NULL){
            free (str);
        }
        if (prop->name != NULL){
            free (prop->name);
        }
        if (prop->value != NULL){
            free (prop->value);
        }
        if (pvalue != NULL){
            free (pvalue);
        }
        return SYNTAX;
    }
    token = strtok (str, ";");
    if (token != NULL){
    for (int i = 0; i < strlen(token); i++){
        token[i] = toupper(token[i]);
        if (token[i] == ' '){
            spaces = true;
        }
    }
    if (spaces == true){
        if (str != NULL){
            free (str);
        }
        if (prop->name != NULL){
            free (prop->name);
        }
        if (prop->value != NULL){
            free (prop->value);
        }
        if (pvalue != NULL){
            free (pvalue);
        }
        return SYNTAX;
    }
    spaces = false;
    prop->name = realloc (prop->name, sizeof (char) * (strlen(token)+1));
    assert (prop->name != NULL);
    strcpy (prop->name, token);
    }
    prop->nparams = semi;
    temp = malloc (sizeof (CalParam*) * semi);
    assert (temp != NULL);
    for (int i = 0; i < semi; i++){
        temp[i] = malloc (sizeof (CalParam) + (sizeof (char*) *nparam[i]));
        assert (temp[i] != NULL);
        temp[i]->next = NULL;
        temp[i]->nvalues = nparam[i];
    }

    do {
        if (con == true){
            pvalue = realloc (pvalue, sizeof (char) * (strlen(pvalue)+strlen(token)+2));
            assert (pvalue != NULL);
            strcat (pvalue, token);
            if (von == true) strcat(pvalue, ",");
            else if (pon == true) strcat(pvalue, ";");
            else if (son == true) strcat(pvalue, ":");
        }
        if (eon == true){
            token = strtok (NULL, "=");
            if (token != NULL){
            if (token[0] != '"'){
                for (int i = 0; i < strlen(token); i++){
                    token[i] = toupper(token[i]);
                    if (token[i] == ' '){
                        spaces = true;
                    }
                }
            }
            if (spaces == true){
                if (str != NULL){
                    free (str);
                }
                if (prop->name != NULL){
                    free (prop->name);
                }
                if (prop->value != NULL){
                    free (prop->value);
                }
                if (pvalue != NULL){
                    free (pvalue);
                }
                for (int i = 0; i < semi; i++){
                    free (temp[i]);
                }
                free (temp);
                return SYNTAX;
            }
            temp[sindex]->name = malloc (sizeof (char) * (strlen(token)+1));
            assert (temp[sindex]->name != NULL);
            strcpy (temp[sindex]->name, token);
            }
            if ((nparam[sindex] - vindex) > 1){
                von = true;
                eon = false;
                pon = false;
                son = false;
            }
            else if ((semi - sindex) > 1){
                pon = true;
                eon = false;
                von = false;
                son = false;
            }
            else {
                von = false;
                eon = false;
                pon = false;
                son = true;
            }
        }
        else if (von == true){
            token = strtok (NULL, ",");
            if (token != NULL){
            if (con == false){
                if (token[0] != '"'){
                    for (int i = 0; i < strlen(token); i++){
//                        token[i] = toupper(token[i]);
                    }
                }
            }
            for (int i = 0; i < strlen(token); i++){
                if (token[i] == '"'){
                    nq++;
                }
            }
            if (nq%2 == 1){
                if (inquo == false){ inquo = true; con = true;}
                else if (inquo == true) inquo = false;
            }
            nq = 0;
            if (inquo == false){
                if (con == false){
                    temp[sindex]->value[vindex] = malloc (sizeof (char) * (strlen(token)+1));
                    assert (temp[sindex]->value[vindex] != NULL);
                    strcpy (temp[sindex]->value[vindex] ,token);
                }
                else if (con == true){
                    pvalue = realloc (pvalue, sizeof (char) * (strlen(pvalue)+strlen(token)+1));
                    assert (pvalue != NULL);
                    strcat (pvalue, token);
                    temp[sindex]->value[vindex] = malloc (sizeof (char) * (strlen(pvalue)+1));
                    assert (temp[sindex]->value[vindex] != NULL);
                    strcpy (temp[sindex]->value[vindex], pvalue);
                    strcpy (pvalue, "");
                    con = false;
                }

                vindex++;
                if ((nparam[sindex] - vindex) > 1){
                    von = true;
                    eon = false;
                    pon = false;
                    son = false;
                }
                else if ((semi - sindex) > 1){
                    pon = true;
                    eon = false;
                    von = false;
                    son = false;
                }
                else {
                    von = false;
                    eon = false;
                    pon = false;
                    son = true;
                }
            }
            }
        }
        else if (pon == true){
            token = strtok (NULL, ";");
            if (token != NULL){
            if (con == false){
                if (token[0] != '"'){
                    for (int i = 0; i < strlen(token); i++){
//                        token[i] = toupper(token[i]);
                    }
                }
            }
            for (int i = 0; i < strlen(token); i++){
                if (token[i] == '"'){
                    nq++;
                }
            }
            if (nq%2 == 1){
                if (inquo == false){ inquo = true; con = true;}
                else if (inquo == true) inquo = false;
            }
            nq = 0;
            if (inquo == false){
                if (con == false){
                    temp[sindex]->value[vindex] = malloc (sizeof (char) * (strlen(token)+1));
                    assert (temp[sindex]->value[vindex] != NULL);
                    strcpy (temp[sindex]->value[vindex], token);
                }
                else if (con == true){
                    pvalue = realloc (pvalue, sizeof (char) * (strlen(pvalue)+strlen(token)+1));
                    assert (pvalue != NULL);
                    strcat (pvalue, token);
                    temp[sindex]->value[vindex] = malloc (sizeof (char) * (strlen(pvalue)+1));
                    assert (temp[sindex]->value[vindex] != NULL);
                    strcpy (temp[sindex]->value[vindex], pvalue);
                    strcpy (pvalue, "");
                    con = false;
                }
                if (prop->param == NULL){
                    prop->param = temp[sindex];
                }
                else {
                    cur = prop->param;
                    while (cur->next != NULL){
                        cur = cur->next;
                    }
                    cur->next = temp[sindex];
                }
                sindex++;
                vindex = 0;
                von = false;
                eon = true;
                pon = false;
                son = false;
            }
            }
        }
        else if (son == true){
            token = strtok (NULL, ":");
            if (token != NULL){
            if (con == false){
                if (token[0] != '"'){
                    for (int i = 0; i < strlen(token); i++){
//                        token[i] = toupper(token[i]);
                    }
                }
            }
            for (int i = 0; i < strlen(token); i++){
                if (token[i] == '"'){
                    nq++;
                }
            }
            }
            if (nq%2 == 1){
                if (inquo == false){
                    inquo = true;
                    con = true;
                }
                else if (inquo == true) inquo = false;
            }
            nq = 0;
            if (inquo == false){
                if (con == false){
                    if (token != NULL){
                    temp[sindex]->value[vindex] = malloc (sizeof (char) * (strlen(token)+1));
                    assert (temp[sindex]->value[vindex] != NULL);
                    strcpy (temp[sindex]->value[vindex], token);
                    }
                }
                else if (con == true){
                    if (token != NULL){
                    pvalue = realloc (pvalue, sizeof (char) * (strlen(pvalue)+strlen(token)+1));
                    assert (pvalue != NULL);
                    strcat (pvalue, token);
                    temp[sindex]->value[vindex] = malloc (sizeof (char) * (strlen(pvalue)+1));
                    assert (temp[sindex]->value[vindex] != NULL);
                    strcpy (temp[sindex]->value[vindex], pvalue);
                    strcpy (pvalue, "");
                    con = false;
                    }
                }
                if (prop->param == NULL){
                    prop->param = temp[sindex];
                }
                else {
                    cur = prop->param;
                    while (cur->next != NULL){
                        cur = cur->next;
                    }
                    cur->next = temp[sindex];
                }
                token = strtok (NULL, "");
                if (token != NULL){
                prop->value = realloc (prop->value, (sizeof (char) * strlen (token) + 1));
                assert (prop->value != NULL);
                strcpy(prop->value, token);
                }
                if ((token = strtok (NULL, "")) == NULL) done = 1;
            }
        }

    } while(token != NULL);
    }

    free (temp);

    if (pvalue != NULL){
        free (pvalue);
        pvalue = NULL;
    }

    }
    free (str);
    return OK;
}

void freeCalComp( CalComp *const comp ){
    if (comp == NULL){
        return;
    }
    if (comp->ncomps > 0){
        for (int i = 0; i < comp->ncomps; i++){
            freeCalComp (comp->comp[i]);
        }
    }

    if (comp->nprops > 0){
        CalProp * headProp = comp->prop;
        for (int i = 0; i < comp->nprops; i++){
            CalProp * tempProp = headProp;
            headProp = tempProp->next;
            if (tempProp != NULL){
                if(tempProp->nparams > 0){
                    CalParam * headPar = tempProp->param;
                    for (int i = 0; i < tempProp->nparams; i++){
                        CalParam * tempPar = headPar;
                        headPar = tempPar->next;
                        if (tempPar != NULL){
                            if (tempPar->nvalues > 0){
                                for (int i = 0; i < tempPar->nvalues; i++){
                                    free (tempPar->value[i]);
                                    tempPar->value[i] = NULL;
                                }
                            }
                            tempPar->next = NULL;
                            free (tempPar->name);
                            tempPar->name = NULL;
                            free (tempPar);
                        }
                    }
                }
                tempProp->next = NULL;
                free (tempProp->value);
                tempProp->value = NULL;
                free (tempProp->name);
                tempProp->name = NULL;
                free (tempProp);
            }
        }
    }
    free (comp->name);
    comp->name = NULL;
    free (comp);
    return;
}

void printCalendar (CalComp * const pcomp, FILE * txtfile){
    if (pcomp->name != NULL){
        fprintf (txtfile, "BEGIN:%s\r\n", pcomp->name);
    }
    CalProp * cur = pcomp->prop;
    while (cur != NULL){
        if (cur->name !=  NULL){
            fprintf(txtfile, "%s", cur->name);
        }
        if (cur->nparams > 0){
            fprintf(txtfile, ";");
        }
        else {
            fprintf(txtfile, ":");
        }
        CalParam * temp = cur->param;
        while (temp != NULL){
            if (temp->name != NULL){
                fprintf(txtfile, "%s=", temp->name);
            }
            for (int i = 0; i < temp->nvalues; i++){
                if (temp->value[i] != NULL){
                    fprintf (txtfile, "%s", temp->value[i]);
                }
                if (temp->nvalues - i == 1){
                    if (temp->next == NULL){
                        fprintf(txtfile, ":");
                    }
                    else {
                        fprintf(txtfile, ";");
                    }
                }
                else {
                    fprintf(txtfile, ",");
                }
            }
            temp = temp->next;
        }
        if (cur->value != NULL){
            fprintf(txtfile, "%s\r\n", cur->value);
        }
        cur = cur->next;
    }
    for (int i = 0; i < pcomp->ncomps; i++){
        printCalendar (pcomp->comp[i], txtfile);
    }
    fprintf (txtfile, "END:%s\r\n", pcomp->name);
}

CalStatus writeCalComp( FILE *const ics, const CalComp *comp ){
    CalStatus stat;
    stat.code = OK;
    stat.lineto = 0;
    stat.linefrom = 0;
    CalStatus st;
    st.code = OK;
    st.lineto = 0;
    st.linefrom = 0;
    int nstr = 0;
    int rem = 0;
    int length = 0;
    char ** strings = NULL;
    char * string = NULL;
    int ret = 0;

    if (comp->name != NULL){
        string = malloc (sizeof (char) * (strlen(comp->name) + 10));
        assert (string != NULL);
        strcpy(string,"BEGIN:");
        strcat (string, comp->name);
        length = strlen(string);
        if (length > FOLD_LEN){
            nstr = ((length-75)/74) + 2;
            rem = (length-75)%74;
            if (rem == 0){
                nstr--;
            }
            strings = malloc (sizeof (char*) * nstr);
            assert (strings != NULL);
            for (int i = 0; i < nstr; i++){
                strings[i] = malloc (sizeof (char) * 78);
                assert (strings[i] != NULL);
                if (i == 0){
                    strncpy (strings[i], string, 75);
                    strings[i][75] = '\0';
                }
                else {
                    strncpy (strings[i], &string[(i*74)+1], 74);
                    strings[i][74] = '\0';
                }
                strcat (strings[i], "\r\n");
                if (i == 0){
                    ret = fprintf (ics, "%s", strings[i]);
                }
                else {
                    ret = fprintf (ics, " %s", strings[i]);
                }
                if (ret < 0){
                    for (int a = 0; a <= i; a++){
                        free (strings[a]);
                    }
                    free (strings);
                    free (string);
                    stat.code = IOERR;
                    return stat;
                }
                stat.lineto++;
                if (i == 0){
                    stat.linefrom++;
                }
            }
            for (int i = 0; i < nstr; i++){free(strings[i]);}
            if (strings != NULL){
                free (strings);
                strings = NULL;
            }
        }
        else {
            strcat(string,"\r\n");
            ret = fprintf (ics, "%s", string);
            if (ret < 0){
                free (string);
                stat.code = IOERR;
                return stat;
            }
            stat.linefrom++;
            stat.lineto++;
        }
        if (string != NULL){
            free (string);
            string = NULL;
        }
    }
    CalProp * cur = comp->prop;
    while (cur != NULL){
        if (cur->name !=  NULL){
            string = malloc (sizeof (char) * (strlen(cur->name) +2));
            assert (string != NULL);
            strcpy (string, cur->name);
        }
        if (cur->nparams > 0){
            strcat (string, ";");
        }
        else {
            strcat (string, ":");
        }
        length = strlen(string);
        CalParam * temp = cur->param;
        while (temp != NULL){
            if (temp->name != NULL){
                string = realloc (string, sizeof (char) * (length + strlen(temp->name)+2));
                assert (string != NULL);
                strcat (string, temp->name);
                strcat (string, "=");
                length = strlen(string);
            }
            for (int i = 0; i < temp->nvalues; i++){
                if (temp->value[i] != NULL){
                    string = realloc (string, sizeof (char) * (length + strlen(temp->value[i])+2));
                    assert (string != NULL);
                    strcat (string, temp->value[i]);
                    length = strlen(string);
                }
                if (temp->nvalues - i == 1){
                    if (temp->next == NULL){
                        strcat(string, ":");
                    }
                    else {
                        strcat(string, ";");
                    }
                }
                else {
                    strcat(string, ",");
                }
                length = strlen(string);
            }
            temp = temp->next;
        }
        if (cur->value != NULL){
            string = realloc (string, sizeof (char) * (length + strlen(cur->value)+3));
            assert (string != NULL);
            strcat (string, cur->value);
            length = strlen(string);
        }
        if (length > FOLD_LEN){
            nstr = ((length-75)/74) + 2;
            rem = (length-75)%74;
            if (rem == 0){
                nstr--;
            }
            strings = malloc (sizeof (char*) * nstr);
            assert (strings != NULL);
            for (int i = 0; i < nstr; i++){
                strings[i] = malloc (sizeof (char) * 78);
                assert (strings[i] != NULL);
                if (i == 0){
                    strncpy (strings[i], string, 75);
                    strings[i][75] = '\0';
                }
                else {
                    strncpy (strings[i], &string[(i*74)+1], 74);
                    strings[i][74] = '\0';
                }
                strcat (strings[i], "\r\n");
                if (i == 0){
                    ret = fprintf (ics, "%s", strings[i]);
                }
                else {
                    ret = fprintf (ics, " %s", strings[i]);
                }
                if (ret < 0){
                    for (int a = 0; a <= i; a++){
                        free (strings[a]);
                    }
                    free (strings);
                    free (string);
                    stat.code = IOERR;
                    return stat;
                }
                stat.lineto++;
                if (i == 0){
                    stat.linefrom++;
                }
            }
            for (int i = 0; i < nstr; i++){free(strings[i]);}
            if (strings != NULL){
                free (strings);
                strings = NULL;
            }
        }
        else {
            strcat(string,"\r\n");
            ret = fprintf (ics, "%s", string);
            if (ret < 0){
                free (string);
                stat.code = IOERR;
                return stat;
            }
            stat.linefrom++;
            stat.lineto++;
        }
        if (string!= NULL){
            free (string);
            string = NULL;
        }
        cur = cur->next;
    }
    for (int i = 0; i < comp->ncomps; i++){
        st = writeCalComp (ics, comp->comp[i]);
        stat.lineto = stat.lineto + st.lineto;
        stat.linefrom = stat.linefrom + st.linefrom;
        stat.code = st.code;
        if (stat.code != OK){
            return stat;
        }
    }

    if (comp->name != NULL){
        string = malloc (sizeof (char) * (strlen(comp->name) + 7));
        assert (string != NULL);
        strcpy (string, "END:");
        strcat (string, comp->name);
        length = strlen (string);
        if (length > FOLD_LEN){
            nstr = ((length-75)/74) + 2;
            rem = (length-75)%74;
            if (rem == 0){
                nstr--;
            }
            strings = malloc (sizeof (char*) * nstr);
            assert (strings != NULL);
            for (int i = 0; i < nstr; i++){
                strings[i] = malloc (sizeof (char) * 78);
                assert (strings[i] != NULL);
                if (i == 0){
                    strncpy (strings[i], string, 75);
                    strings[i][75] = '\0';
                }
                else {
                    strncpy (strings[i], &string[(i*74)+1], 74);
                    strings[i][74] = '\0';
                }
                strcat (strings[i], "\r\n");
                if (i == 0){
                    ret = fprintf (ics, "%s", strings[i]);
                }
                else {
                    ret = fprintf (ics, " %s", strings[i]);
                }
                if (ret < 0){
                    for (int a = 0; a <= i; a++){
                        free (strings[a]);
                    }
                    free (strings);
                    free (string);
                    stat.code = IOERR;
                    return stat;
                }
                stat.lineto++;
                if (i == 0){
                    stat.linefrom++;
                }
            }
            for (int i = 0; i < nstr; i++){free(strings[i]);}
            if (strings != NULL){
                free (strings);
                strings = NULL;
            }
        }
        else {
            strcat(string,"\r\n");
            ret = fprintf (ics, "%s", string);
            if (ret < 0){
                free (string);
                stat.code = IOERR;
                return stat;
            }
            stat.linefrom++;
            stat.lineto++;
        }
        free (string);
        string = NULL;
    }
    if (string != NULL){
        free (string);
    }
    return stat;
}
