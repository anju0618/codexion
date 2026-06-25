/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code3_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 00:37:07 by amakino           #+#    #+#             */
/*   Updated: 2026/06/26 02:15:35 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief: 個別のドングルとコーダーの情報を初期化する。
 * @detail: 各ドングルにMutexと条件変数（Cond）を初期化・割り当て,待機キュー用のメモリを確保
 * @detail: 各コーダーに対して左右のドングルのポインタ（left_dongle, right_dongle）をリンクさせます。
**/
static int	init_single_element(t_config *config, t_dongle *dongles,
				t_coder *coders, int i)
{
	dongles[i].id = i + 1;
	pthread_mutex_init(&(dongles[i].mutex), NULL);
	pthread_cond_init(&(dongles[i].cond), NULL);
	dongles[i].cooldown_end = 0;
	dongles[i].queue = malloc(sizeof(int) * config->num_coders);
	if (!dongles[i].queue)
		return (0);
	dongles[i].queue_size = 0;
	dongles[i].holder_id = 0;
	coders[i].id = i + 1;
	coders[i].compile_count = 0;
	coders[i].deadline = config->start_time + config->time_burnout;
	coders[i].config = config;
	coders[i].left_dongle = &(dongles[i]);
	coders[i].right_dongle = &(dongles[(i + 1) % config->num_coders]);
	return (1);
}

/**
 * @brief: 確保された構造体配列全体をループ処理で初期化
 * @detail: init_single_elementを繰り返し呼び出す
 * @detail: メモリ確保エラーなどが発生した場合、それまでに確保したキューのメモリをすべて解放してからエラー終了
 */
int	setup_elements(t_config *config, t_dongle *dongles, t_coder *coders)
{
	int	i;

	i = 0;
	while (i < config->num_coders)
	{
		if (!init_single_element(config, dongles, coders, i))
		{
			while (--i >= 0)
				free(dongles[i].queue);
			return (0);
		}
		i++;
	}
	return (1);
}

/**
 * @brief: 初期化処理のトリガー.タイムスタンプ基準点の記録,全体のメモリ確保
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
		fprintf(stderr, "Error: malloc failed\n");
		return (0);
	}
	if (!setup_elements(config, *dongles, *coders))
	{
		free(*dongles);
		free(*coders);
		fprintf(stderr, "Error: queue malloc failed\n");
		return (0);
	}
	return (1);
}
