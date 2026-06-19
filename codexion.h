/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 00:27:04 by amakino           #+#    #+#             */
/*   Updated: 2026/06/20 00:57:16 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

typedef struct s_config
{
	int		num_coders;
	int		time_burnout;
	int		time_compile;
	int		time_debug;
	int		time_refactor;
	int		num_compiles_req;
	int		dongle_cooldown;
	char	*scheduler;
}	t_config;

/** code2_parsing.c **/
int	is_numeric_str(char *str);
int	parse_arguments(t_config *config, char **av);
int	validate_conf(t_config *config, char **av);
