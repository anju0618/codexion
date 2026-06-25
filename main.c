/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 00:26:33 by amakino           #+#    #+#             */
/*   Updated: 2026/06/26 02:27:22 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief シミュレーションの終了フラグ（stop_flag）をMutexで安全に読み取る
 * @detail: 複数のコーダースレッドやモニタースレッドが同時に終了条件を参照するため、Data Raceを防ぐためのヘルパー関数
**/
int	check_stop_flag(t_config *config)
{
	int	res;

	pthread_mutex_lock(&config->state_mutex);
	res = config->stop_flag;
	pthread_mutex_unlock(&config->state_mutex);
	return (res);
}

/**
 * @brief コーダーの状態変化（"is compiling"など）をスレッドセーフに標準出力へ印字する。
 * @detail: メッセージが端末上で混ざらないよう print_mutex で保護.
 * @detail: また、印字直前に stop_flag を確認.すでに誰かがBurnoutしている場合は不要なログを出力しない
**/
void	print_state(t_coder *coder, const char *state)
{
	long long	timestamp;
	int			stop;

	pthread_mutex_lock(&coder->config->state_mutex);
	stop = coder->config->stop_flag;
	pthread_mutex_unlock(&coder->config->state_mutex);
	pthread_mutex_lock(&coder->config->print_mutex);
	if (!stop || strcmp(state, "burned out") == 0)
	{
		timestamp = get_time_ms() - coder->config->start_time;
		printf("%lld %d %s\n", timestamp, coder->id, state);
	}
	pthread_mutex_unlock(&coder->config->print_mutex);
}

/**
 * @brief 各コーダーのコンパイル→デバッグ→リファクタリングをループ実行するスレッド用関数
 * @detail: stop_flag が立つまで無限ループします.
 * @detail: ドングル取得後に自身のdeadlineを更新.コンパイル完了後にcompile_countをインクリメント。
 * 			各状態の待機にはOSのブレを吸収する precise_usleep を使用します。
**/
void	*coder_routine(void *arg)
{
	t_coder		*coder;

	coder = (t_coder *)arg;
	while (!check_stop_flag(coder->config))
	{
		acquire_dongles(coder);
		pthread_mutex_lock(&coder->config->state_mutex);
		coder->deadline = get_time_ms() + coder->config->time_burnout;
		pthread_mutex_unlock(&coder->config->state_mutex);
		print_state(coder, "is compiling");
		precise_usleep(coder->config->time_compile);
		pthread_mutex_lock(&coder->config->state_mutex);
		coder->compile_count++;
		pthread_mutex_unlock(&coder->config->state_mutex);
		release_dongles(coder);
		print_state(coder, "is debugging");
		precise_usleep(coder->config->time_debug);
		print_state(coder, "is refactoring");
		precise_usleep(coder->config->time_refactor);
	}
	return (NULL);
}

/**
 * @brief モニタースレッドと各コーダースレッドを生成.全スレッドの終了（join）を待機
 * @detail: pthread_createに失敗した場合は直ちにエラー
**/
int	run_simulation(t_config *config, t_coder *coders)
{
	pthread_t	monitor_id;
	int			i;

	if (pthread_create(&monitor_id, NULL, monitor_routine, coders))
		return (0);
	i = 0;
	while (i < config->num_coders)
	{
		if (pthread_create(&coders[i].thread_id, NULL, coder_routine,
				&coders[i]))
			return (0);
		i++;
	}
	i = 0;
	while (i < config->num_coders)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
	pthread_join(monitor_id, NULL);
	return (1);
}

int	main(int ac, char **av)
{
	t_config	config;
	t_dongle	*dongles;
	t_coder		*coders;
	int			i;

	if (ac != 9)
		return (fprintf(stderr, "Error: need 8 args.\n"), 1);
	if (!init(&config, &dongles, &coders, av))
		return (1);
	pthread_mutex_init(&config.print_mutex, NULL);
	pthread_mutex_init(&config.state_mutex, NULL);
	run_simulation(&config, coders);
	pthread_mutex_destroy(&config.print_mutex);
	pthread_mutex_destroy(&config.state_mutex);
	i = 0;
	while (i < config.num_coders)
	{
		pthread_mutex_destroy(&dongles[i].mutex);
		pthread_cond_destroy(&dongles[i].cond);
		free(dongles[i].queue);
		i++;
	}
	free(dongles);
	free(coders);
	return (0);
}
