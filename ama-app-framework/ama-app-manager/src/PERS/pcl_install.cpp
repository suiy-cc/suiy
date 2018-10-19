/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "stdio.h"
#include "string.h"
#include "dlt/dlt.h"
#include <getopt.h>
#include "pers_installer.h"
#include "pers_config.h"

static const char * shortOpts = "i:h";
static const struct option longOpts[] = {
    { "install", no_argument, NULL, 'i' },
    { "help", no_argument, NULL, 'h' }
    // { "language", required_argument, NULL, 'l' },
    // { "output", required_argument, NULL, 'o' },
    // { "verbose", no_argument, NULL, 'v' },
    // { "randomize", no_argument, NULL, 0 },
    // { "help", no_argument, NULL, 'h' },
    // { NULL, no_argument, NULL, 0 }
};

char * optArg;

// int main(int argc, char * argv[])
// {
//     int c;
//     PersInstaller pi;
//     PersConfig pc("/home/pers_config.xml", "/home");
//     bool res;
//     while((c = getopt_long (argc, argv, shortOpts, longOpts, NULL)) != -1)
//     {
//         switch (c)
//         {
//         case 'i':
//             printf("Pers Install file. %s \n", optarg);
//             optArg = optarg;
//             // pi.setPath(optArg);
//             // res = pi.install();
//             // if (res) {
//             //     printf("Pers Install success !");
//             // } else {
//             //     printf("Pers Install failed !\");
//             // }
//             // PersConfig::extractFileVersion("pcl-public-1110.54.1.tar.gz");
//             pc.installPersFiles();
//             break;
//         case 'h':
//             printf("Pers Help .\n");
//             break;
//         // case 'l':
//         //     optArg = optarg;
//         //     printf("Our love is %s!/n", optArg);
//         //     break;
//         default:
//             printf("invalid argument !");
//             break;
//         }
//     }
//     return 0;
// }
