/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code3_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/22 00:37:07 by amakino           #+#    #+#             */
/*   Updated: 2026/06/22 00:42:24 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init(t_config *config, t_dongle **dongles, t_coder **coders, char **av)
{
	if (!parse_arguments(config, av))
		return (0);
	*dongles = malloc(sizeof(t_dongle) * config->num_coders);
	*coders = malloc(sizeof(t_coder) * config->num_coders);
	if (!(*dongles) || !(*coders))
	{
		free(*dongles);
		free(*coders);
		return (fprintf(stderr, "Error: malloc failed\n"), 0);
	}
	return (1);
}
