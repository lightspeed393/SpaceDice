#include <stdio.h>
#include "stuff.h"
#include <jansson.h>
#include <QDebug>
#include "functions.h"
#include <string.h>

/*
#define v_rpcuser "bitcoinrpc"
#define v_rpcpassword "password"
*/

char v_rpcuser[MAXBUF];
char v_rpcpassword[MAXBUF];

#define v_rpcport 30177

struct config configstruct;

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

void init_config() {

    char userhome[MAXBUF];
    #ifndef _WIN32
        snprintf(userhome, MAXBUF, "%s/.komodo/KMDICE/KMDICE.conf", getenv("HOME"));
    #else
        snprintf(userhome, MAXBUF, "%s\\Komodo\\KMDICE\\KMDICE.conf", getenv("APPDATA"));
    #endif // !_WIN32

    configstruct = get_config(userhome);
    //printf("%s\n", configstruct.rpcuser);
    //printf("%s\n", configstruct.rpcpassword);
    qDebug() << configstruct.rpcuser << configstruct.rpcpassword;
    strncpy(v_rpcuser,configstruct.rpcuser,MAXBUF);
    strncpy(v_rpcpassword,configstruct.rpcpassword,MAXBUF);
}

struct config get_config(char *filename)
{
        struct config configstruct;
        memset(&configstruct, 0, sizeof(configstruct)); // (???) check release build

        FILE *file = fopen (filename, "r");

        if (file != NULL)
        {
                char line[MAXBUF];
                int i = 0;

                while(fgets(line, sizeof(line), file) != NULL)
                {
                        char *cfline, *key;
                        cfline = strstr((char *)line,DELIM);
                        if (cfline) {
                            *cfline = 0;
                            key = trimwhitespace(line);
                            //printf("[1] key = %s\n",key);
                            cfline = cfline + strlen(DELIM);
                            cfline = trimwhitespace(cfline);
                            //printf("[2] value = %s\n",cfline);
                            if (strcmp(key,"rpcuser") == 0)
                                memcpy(configstruct.rpcuser,cfline,strlen(cfline));
                            if (strcmp(key,"rpcpassword") == 0)
                                memcpy(configstruct.rpcpassword,cfline,strlen(cfline));
                            if (strcmp(key,"rpcport") == 0)
                                configstruct.rpcport = atoi(cfline);
                        }
                        i++;
                }
                fclose(file);
        }
        return configstruct;

}



t_getinfo getInfo() {

    t_getinfo res;
    char *txt, request[256];
    double balance = 0.0;
    json_t *root, *result;
    json_error_t error;

    memset(&res, 0, sizeof(res));
    snprintf(request, 256, "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"getinfo\", \"params\": [] }");
    txt = daemon_request("localhost", v_rpcport, v_rpcuser, v_rpcpassword, request);
    qDebug() << txt;
    if (txt) {
        root = json_loads(txt, 0, &error);
        free(txt);
        if (root) {
            if json_is_null(json_object_get(root, "error")) {
                result = json_object_get(root, "result");
                res.balance = json_number_value(json_object_get(result, "balance"));
                res.blocks = json_number_value(json_object_get(result, "blocks"));
                res.longestchain = json_number_value(json_object_get(result, "longestchain"));
            }
            json_decref(root);
        }
    }

    return (res);
}

t_diceinfo getDiceInfo() {
    t_diceinfo res;
    char *txt, request[256];
    char fundingtxid[65];
    json_t *root, *result;
    json_error_t error;

    // obtain fundingtx ids
    snprintf(request, 256, "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"dicelist\", \"params\": [] }");
    txt = daemon_request("localhost", v_rpcport, v_rpcuser, v_rpcpassword, request);
    //qDebug() << txt;
    memset(&res, 0, sizeof(res)); fundingtxid[0] = '\0';

    if (txt) {
        root = json_loads(txt, 0, &error);
        free(txt);
        if (root) {
            if json_is_null(json_object_get(root, "error")) {
                result = json_object_get(root, "result");
                if json_is_array(result) {
                    // qDebug() << json_array_size(result); // total size of fundingtxids array
                    strncpy(fundingtxid, json_string_value(json_array_get(result, 0)),sizeof(fundingtxid));
                }
            }
            json_decref(root);
        }
    }

    if (strlen(fundingtxid) != 0) {
        strncpy(res.fundingtxid, fundingtxid, sizeof(fundingtxid));
        snprintf(request, 256, "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"diceinfo\", \"params\": [\"%s\"] }", fundingtxid);
        txt = daemon_request("localhost", v_rpcport, v_rpcuser, v_rpcpassword, request);
        qDebug() << txt;
        root = json_loads(txt, 0, &error);
        free(txt);
        if (root) {
            if json_is_null(json_object_get(root, "error")) {
                result = json_object_get(root, "result");
                strncpy(res.name,json_string_value(json_object_get(result,"name")),sizeof(res.name));
                res.sbits = json_integer_value(json_object_get(result,"sbits"));
                res.minbet = QString(json_string_value(json_object_get(result,"minbet"))).toDouble();
                res.maxbet = QString(json_string_value(json_object_get(result,"maxbet"))).toDouble();
                res.maxodds = json_integer_value(json_object_get(result,"maxodds"));
                res.timeoutblocks = json_integer_value(json_object_get(result,"timeoutblocks"));
                res.funding = QString(json_string_value(json_object_get(result,"funding"))).toDouble();
            }
            json_decref(root);
        }

    } else
        strncpy(res.fundingtxid, "error", sizeof(fundingtxid));


    return res;
}

char* dicebet(char *rawtx, int len, char *name, char* fundingtxid, double amount, uint64_t odds) {
// rawtx=$(./komodo-cli -ac_name=KMDICE dicebet $name $fundingtxid $amount $odds | jq -r .hex)
    char *txt, request[256];
    char amount_str[65], odds_str[65];

    json_t *root, *result;
    json_error_t error;

    rawtx[0] = 0;

    // we should find normal way to conver double -> string with dot as decimal separator
    const struct lconv* loc = localeconv();
    snprintf(amount_str,sizeof(amount_str),"%.8f",amount);
    snprintf(odds_str,sizeof(odds_str),"%llu", odds);
    if (loc->decimal_point[0] == ',') {
        for (int i=0; i<strlen(amount_str); i++) if (amount_str[i] == ',') { amount_str[i] = '.'; break; }
        for (int i=0; i<strlen(odds_str); i++) if (odds_str[i] == ',') { odds_str[i] = '.'; break; }
    }

    snprintf(request, 256, "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"dicebet\", \"params\": [\"%s\",\"%s\",\"%s\",\"%s\"] }", name, fundingtxid, amount_str, odds_str);
    //qDebug() << request;
    txt = daemon_request("localhost", v_rpcport, v_rpcuser, v_rpcpassword, request);
    //qDebug() << txt;
    root = json_loads(txt, 0, &error);
    free(txt);
    if (root) {
            result = json_object_get(root, "result");
            if (strcmp(json_string_value(json_object_get(result,"result")),"success") == 0) {
            strncpy(rawtx,json_string_value(json_object_get(result,"hex")),len);
            // qDebug() << rawtx;
            } // TODO: in case of error we should somehow pass it to caller
            json_decref(root);
    }

    if (rawtx[0] != 0)
        return rawtx;
    else
        return NULL;
}

char* sendrawtx(char *txid, char *rawtx, int rawtx_len) {
    char *txt, request[32768];

    json_t *root, *result;
    json_error_t error;

    txid[0] = '\0';

    snprintf(request, sizeof(request), "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"sendrawtransaction\", \"params\": [\"%s\"] }", rawtx);
    txt = daemon_request("localhost", v_rpcport, v_rpcuser, v_rpcpassword, request);
    // qDebug() << txt;
    if (txt) {
        root = json_loads(txt, 0, &error);
        free(txt);
        if (root) {
            if json_is_null(json_object_get(root, "error")) {
                result = json_object_get(root, "result");
                strncpy(txid, json_string_value(result), 65);
                // qDebug() << json_string_value(result) << txid;
            }
            json_decref(root);
        }
    }

    if (rawtx[0] != 0)
        return rawtx;
    else
        return NULL;

}

int dicestatus(char *txid, double *won, char *name, char* fundingtxid) {
    /*
     * return:
     * -1 - error
     * 0 - loss
     * 1 - win
     * 2 - bet still pending */

    int res = -1; *won = 0;
    char *txt, request[256], status[256];

    json_t *root, *result, *result_inside;
    json_error_t error;

    // name fundingtxid txid

    snprintf(request, sizeof(request), "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"dicestatus\", \"params\": [\"%s\", \"%s\", \"%s\"] }", name, fundingtxid, txid);
    txt = daemon_request("localhost", v_rpcport, v_rpcuser, v_rpcpassword, request);
    //qDebug() << "[BET] " << txt;
    if (txt) {
        root = json_loads(txt, 0, &error);
        free(txt);
        if (root) {
            if json_is_null(json_object_get(root, "error")) {
                result = json_object_get(root, "result");
                //qDebug() << "[BET] " << json_string_value(json_object_get(result, "status"));
                strncpy(status, json_string_value(json_object_get(result, "status")), sizeof(status));
                if (strcmp(status, "win") == 0) {
                    res = 1;
                    *won = json_number_value(json_object_get(result, "won"));
                } else if (strcmp(status, "loss") == 0) {
                    res = 0;
                } else if (strcmp(status, "bet still pending") == 0) {
                    res = 2;
                }
            }
            json_decref(root);
        }
    }

    return res;


}
