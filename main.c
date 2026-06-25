/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 00:26:33 by amakino           #+#    #+#             */
/*   Updated: 2026/06/22 00:42:30 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_config	*config;

	coder = (t_coder *)arg;
	config = coder->config;
	while (1)
	{
		acquire_dongles(coder);
		coder->deadline = get_time_ms() + config->time_burnout;
		print_state(coder, "is compiling");
		precise_usleep(config->time_compile);
		coder->compile_count++;
		release_dongles(coder);
		print_state(coder, "is debugging");
		precise_usleep(config->time_debug);
		print_state(coder, "is refactoring");
		precise_usleep(config->time_refactor);
	}
	if (config->stop_flag)
		return (NULL);
	return (NULL);
}

int	run_simulation(t_config *config, t_coder *coders)
{
	int	i;

	i = 0;
	while (i < config->num_coders)
	{
		if (pthread_create(&coders[i].thread_id, NULL,
				coder_routine, &coders[i]))
		{
			fprintf(stderr, "Error: Failed to create thread\n");
			return (0);
		}
		i++;
	}
	i = 0;
	while (i < config->num_coders)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
	return (1);
}

int	main(int ac, char **av)
{
	t_config	config;
	t_dongle	*dongles;
	t_coder		*coders;

	if (ac != 9)
		return (fprintf(stderr, "Error: need 8 args.\n"), 1);
	if (!init(&config, &dongles, &coders, av))
		return (1);
	pthread_mutex_init(&config.print_mutex, NULL);
	run_simulation(&config, coders);
	pthread_mutex_destroy(&config.print_mutex);
	free(dongles);
	free(coders);
	return (0);
}
