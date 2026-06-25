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
	timestamp = get_time_ms() - coder->config->start_time;
	printf("%lld %d %s\n", timestamp, coder->id, state);	
	pthread_mutex_unlock(&coder->config->print_mutex);
}

void    *coder_routine(void *arg)
{
	t_coder		*coder;
	t_config	*config;
	coder = (t_coder *)arg;
	config = coder->config;

	while (1)
	{
		if (coder->id % 2 != 0)
		{
			pthread_mutex_lock(&coder->left_dongle->mutex);
			print_state(coder, "has taken a dongle");
			pthread_mutex_lock(&coder->right_dongle->mutex);
			print_state(coder, "has taken a dongle");
		}
		else
		{
			pthread_mutex_lock(&coder->right_dongle->mutex);
			print_state(coder, "has taken a dongle");
			pthread_mutex_lock(&coder->left_dongle->mutex);
			print_state(coder, "has taken a dongle");
		}

		print_state(coder, "is compiling");
		precise_usleep(config->time_compile);
		coder->compile_count++;
		if (coder->id % 2 != 0)
		{
			pthread_mutex_unlock(&coder->right_dongle->mutex);
			pthread_mutex_unlock(&coder->left_dongle->mutex);
		}
		else
		{
			pthread_mutex_unlock(&coder->left_dongle->mutex);
			pthread_mutex_unlock(&coder->right_dongle->mutex);
		}
		print_state(coder, "is debugging");
		precise_usleep(config->time_debug);
		print_state(coder, "is refactoring");
		precise_usleep(config->time_refactor);
	}
	return (NULL);
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
	config.start_time = get_time_ms();
	i = 0;
	while (i < config.num_coders)
	{
		if (pthread_create(&coders[i].thread_id, NULL, coder_routine, &coders[i]) != 0)
		{
			fprintf(stderr, "Error: Failed to create thread\n");
			return (1);
		}
		i++;
	}
	i = 0;
	while (i < config.num_coders)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}

	pthread_mutex_destroy(&config.print_mutex);
	free(dongles);
	free(coders);
	return (0);
}

