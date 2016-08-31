/* horst - Highly Optimized Radio Scanning Tool
 *
 * Copyright (C) 2005-2016 Bruno Randolf (br1@einfach.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/******************* PROBE *******************/

#include <stdlib.h>

#include "display.h"
#include "main.h"
#include "util.h"
#include "wlan_util.h"

#define COL_NO      1
#define COL_CHAN	(COL_NO + 3)
#define COL_SIG		(COL_CHAN + 4)
#define COL_SOURCE	(COL_SIG + 4)
#define COL_WIDTH	(COL_SOURCE + 18)
#define COL_TIME	(COL_WIDTH + 11)
#define COL_TWO     (COL_TIME+9 + 1)

static void update_node_info(WINDOW *win, int num, int line, int col, struct node_info* n)
{
    mvwprintw(win, line, col*COL_TWO + COL_NO, "%2d ", num);
    if (n->wlan_channel)
        mvwprintw(win, line, col*COL_TWO + COL_CHAN, "%3d", n->wlan_channel); // 信道
    mvwprintw(win, line, col*COL_TWO + COL_SIG, "%3d", -ewma_read(&n->phy_sig_avg)); // 信号强度
    mvwprintw(win, line, col*COL_TWO + COL_SOURCE, "%-17s", mac_name_lookup(n->wlan_src, 0)); // mac地址
        // 模式、频宽
    mvwprintw(win, line, col*COL_TWO + COL_WIDTH, "%-2s %-3s",
        get_80211std(n->wlan_chan_width, n->wlan_channel),
        (n->wlan_chan_width == CHAN_WIDTH_UNSPEC ||
         n->wlan_chan_width == CHAN_WIDTH_20_NOHT) ? "20" :
        channel_width_string_short(n->wlan_chan_width, n->wlan_ht40plus));
    // 空间流
    if (n->wlan_rx_streams)
        wprintw(win, " %dx%d", n->wlan_tx_streams, n->wlan_rx_streams);
    
    char buf[9] = {0};
    strftime(buf, 9, "%H:%M:%S", localtime(&n->last_ts));
    mvwprintw(win, line, col*COL_TWO + COL_TIME, "%s", buf);
}

void update_probe_win(WINDOW *win)
{
	int line = 1;
    int total_num = 0;
	struct node_info* n;

	werase(win);
	wattron(win, WHITE);
	wattroff(win, A_BOLD);
	box(win, 0 , 0);
	print_centered(win, 0, COLS, " PROBE ");

	mvwprintw(win, 1, COL_NO, "No");
	mvwprintw(win, 1, COL_CHAN, "Cha");
	mvwprintw(win, 1, COL_SIG, "Sig");
	mvwprintw(win, 1, COL_SOURCE, "TRANSMITTER");
	mvwprintw(win, 1, COL_WIDTH, "ST-MHz-TxR");
    mvwprintw(win, 1, COL_TIME, "TIME");

	mvwprintw(win, 1, COL_TWO+COL_NO, "No");
	mvwprintw(win, 1, COL_TWO+COL_CHAN, "Cha");
	mvwprintw(win, 1, COL_TWO+COL_SIG, "Sig");
	mvwprintw(win, 1, COL_TWO+COL_SOURCE, "TRANSMITTER");
	mvwprintw(win, 1, COL_TWO+COL_WIDTH, "ST-MHz-TxR");
    mvwprintw(win, 1, COL_TWO+COL_TIME, "TIME");


    list_for_each(&nodes, n, list) {
		if (!(n->wlan_mode &  WLAN_MODE_PROBE))
			continue;
		line++;
        total_num++;

        if (line > LINES - 3) {
            break; /* prevent overdraw of last line */
        }
        if (total_num%2) {
            update_node_info(win, total_num, line, 0, n);
        } else {
            //line--;
            update_node_info(win, total_num, --line, 1, n);
        }
        mvwprintw(win, LINES - 3, 1, "line: %d total: %d", line, total_num);
	}

	wnoutrefresh(win);
}

