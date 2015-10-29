/*
 *  Copyright (C) 2015 Adrien Vergé
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <yaml.h>

int read_config()
{
    FILE *fh = fopen("config.yml", "r");
    yaml_parser_t parser;
    yaml_token_t  token;   /* new variable */

    /* Initialize parser */
    if (!yaml_parser_initialize(&parser))
        fputs("Failed to initialize parser!\n", stderr);
    if (fh == NULL)
        fputs("Failed to open file!\n", stderr);

    /* Set input file */
    yaml_parser_set_input_file(&parser, fh);

    /* BEGIN new code */
    do {
        yaml_parser_scan(&parser, &token);
        switch (token.type) {
        /* Stream start/end */
        case YAML_STREAM_START_TOKEN:
            puts("STREAM START");
            break;
        case YAML_STREAM_END_TOKEN:
            puts("STREAM END");
            break;
        /* Token types (read before actual token) */
        case YAML_KEY_TOKEN:
            printf("(Key token)   ");
            break;
        case YAML_VALUE_TOKEN:
            printf("(Value token) ");
            break;
        /* Block delimeters */
        case YAML_BLOCK_SEQUENCE_START_TOKEN:
            puts("<b>Start Block (Sequence)</b>");
            break;
        case YAML_BLOCK_ENTRY_TOKEN:
            puts("<b>Start Block (Entry)</b>");
            break;
        case YAML_BLOCK_END_TOKEN:
            puts("<b>End block</b>");
            break;
        /* Data */
        case YAML_BLOCK_MAPPING_START_TOKEN:
            puts("[Block mapping]");
            break;
        case YAML_SCALAR_TOKEN:
            printf("scalar %s \n", token.data.scalar.value);
            break;
        /* Others */
        default:
            printf("Got token of type %d\n", token.type);
        }
        if (token.type != YAML_STREAM_END_TOKEN)
            yaml_token_delete(&token);
    } while (token.type != YAML_STREAM_END_TOKEN);

    yaml_token_delete(&token);
    /* END new code */

    /* Cleanup */
    yaml_parser_delete(&parser);
    fclose(fh);
    return 0;
}
