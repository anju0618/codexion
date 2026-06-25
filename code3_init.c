/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code3_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 00:37:07 by amakino           #+#    #+#             */
/*   Updated: 2026/06/22 01:12:57 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/*配列への値代入*/
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
 * @brief 初期化
 * @return 0 or 1
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
