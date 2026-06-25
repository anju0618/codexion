/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code3_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 00:37:07 by amakino           #+#    #+#             */
/*   Updated: 2026/06/25 17:40:26 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief 確保された各構造体配列の各メンバに初期値およびドングリポインタを紐付ける
 * @param config 設定構造体ポインタ
 * @param dongles ドングリ構造体配列
 * @param coders コーダー構造体配列
 */
void	setup_elements(t_config *config, t_dongle *dongles, t_coder *coders)
{
	int	i;

	i = 0;
	while (i < config->num_coders)
	{
		dongles[i].id = i + 1;
		pthread_mutex_init(&(dongles[i].mutex), NULL);
		pthread_cond_init(&(dongles[i].cond), NULL);
		dongles[i].cooldown_end = 0;
		dongles[i].queue = malloc(sizeof(int) * config->num_coders);
		dongles[i].queue_size = 0;
		dongles[i].holder_id = 0;
		coders[i].id = i + 1;
		coders[i].compile_count = 0;
		coders[i].deadline = config->start_time + config->time_burnout;
		coders[i].config = config;
		coders[i].left_dongle = &(dongles[i]);
		coders[i].right_dongle = &(dongles[(i + 1) % config->num_coders]);
		i++;
	}
}

/**
 * @brief パースのトリガーを引き、タイムスタンプ基準点の記録、及びメモリ確保・初期化
 * @param config 設定構造体へのポインタ
 * @param dongles ドングリポインタの受取用アドレス
 * @param coders コーダーポインタの受取用アドレス
 * @param av コマンドライン引数
 * @return 1(ok) / 0(Error)
 */
int	init(t_config *config, t_dongle **dongles, t_coder **coders, char **av)
{
	if (!parse_arguments(config, av))
		return (0);
	config->start_time = get_time_ms();
	config->stop_flag = 0;
	*dongles = malloc(sizeof(t_dongle) * config->num_coders);
	*coders = malloc(sizeof(t_coder) * config->num_coders);
	if (!(*dongles) || !(*coders))
	{
		free(*dongles);
		free(*coders);
		return (fprintf(stderr, "Error: malloc failed\n"), 0);
	}
	setup_elements(config, *dongles, *coders);
	return (1);
}
