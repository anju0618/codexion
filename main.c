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

void	print_state(t_coder *coder, const char *state)
{
	long long	timestamp;

	pthread_mutex_lock(&coder->config->print_mutex);
	if (!coder->config->stop_flag || strcmp(state, "burned out") == 0)
	{
		timestamp = get_time_ms() - coder->config->start_time;
		printf("%lld %d %s\n", timestamp, coder->id, state);
	}
	pthread_mutex_unlock(&coder->config->print_mutex);
}

void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_config	*config;

	coder = (t_coder *)arg;
	config = coder->config;
	while (!config->stop_flag)
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
	return (NULL);
}

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
