/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 00:26:33 by amakino           #+#    #+#             */
/*   Updated: 2026/06/20 01:06:16 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int ac, char **av)
{
	t_config	config;

	if (ac != 9)
		return (fprintf (stderr, "Error: need 8 args."), 1);
	if (!parse_arguments(&config, av))
		return (1);
	return (0);
}
