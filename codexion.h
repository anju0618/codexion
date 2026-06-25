/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 00:27:04 by amakino           #+#    #+#             */
/*   Updated: 2026/06/26 02:27:14 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <unistd.h>
# include <stdio.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

/**
 * @struct s_config
 * @brief シミュレーション全体の設定値を保持する構造体
 * * すべての引数パラメータや、共有する終了フラグ、出力保護用Mutexを格納します。
 */
typedef struct s_config
{
	int				num_coders;
	int				time_burnout;
	int				time_compile;
	int				time_debug;
	int				time_refactor;
	int				num_compile_req;
	int				dongle_cooldown;
	char			*scheduler;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	state_mutex;
	long long		start_time;
	int				stop_flag;
}	t_config;

/**
 * @struct s_dongle
 * @brief 各ドングリを管理する構造体
 */
typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	long long		cooldown_end;
	int				holder_id;
	int				*queue;
	int				queue_size;
}	t_dongle;

/**
 * @struct s_coder
 * @brief 各コーダー（個別スレッド）のをポインタで管理する構造体
 */
typedef struct s_coder
{
	int				id;
	pthread_t		thread_id;
	int				compile_count;
	long long		deadline;
	long long		request_time;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_config		*config;
}	t_coder;
/**main.c**/
int			check_stop_flag(t_config *config);
void		print_state(t_coder *coder, const char *state);
int			run_simulation(t_config *config, t_coder *coders);
/** code2_parsing.c **/
int			is_numeric_str(char *str);
int			parse_arguments(t_config *config, char **av);
int			validate_conf(t_config *config, char **av);
/** code3_init.c **/
int			init(t_config *config, t_dongle **dongles,
				t_coder **coders, char **av);
int			setup_elements(t_config *config,
				t_dongle *dongles, t_coder *coders);
/** code4_utils.c **/
long long	get_time_ms(void);
void		precise_usleep(long long target_ms);
/** code5_actions.c **/
void		acquire_dongles(t_coder *coder);
void		release_dongles(t_coder *coder);
/** code6_monitor.c **/
void		*monitor_routine(void *arg);
/** code7_queue.c **/
int			is_higher_priority(t_coder *a, t_coder *b, char *scheduler);
void		push_queue(t_dongle *dongle, t_coder *coders, int coder_id);
int			pop_queue(t_dongle *dongle, t_coder *coders);

#endif
