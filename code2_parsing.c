/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code2_parsing.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 00:41:37 by amakino           #+#    #+#             */
/*   Updated: 2026/06/26 13:23:51 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief 文字列が「正の整数のみ」で構成されているかを判定する
 */
int	is_numeric_str(char *str)
{
	int	i;
	int	len;

	i = 0;
	if (!str || str[i] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	i = 0;
	while (str[i] == '0')
		i++;
	len = strlen(&str[i]);
	if (len > 10)
		return (0);
	if (len == 10 && strcmp(&str[i], "2147483647") > 0)
		return (0);
	return (1);
}

/**
 * @brief 設定値の妥当性を検証し、スケジューラをセットする
 * @detail: 1番目から7番目までの引数がすべて数字であることを確認後、atoiで変換して構造体に格納します。
 */
int	parse_arguments(t_config *config, char **av)
{
	int	i;

	i = 1;
	while (av[i] && i < 8)
	{
		if (!is_numeric_str(av[i]))
		{
			fprintf(stderr, "Error: arg %d must be numeric\n", i);
			return (0);
		}
		i++;
	}
	config->num_coders = atoi(av[1]);
	config->time_burnout = atoi(av[2]);
	config->time_compile = atoi(av[3]);
	config->time_debug = atoi(av[4]);
	config->time_refactor = atoi(av[5]);
	config->num_compile_req = atoi(av[6]);
	config->dongle_cooldown = atoi(av[7]);
	if (!validate_conf(config, av))
		return (0);
	return (1);
}

/**
 * @brief コマンドライン引数をパースし、型チェックを行って設定構造体に代入する
 * @detail: コーダーの数が0以下,第8引数の文字列がに"fifo"または"edf"であるか,不正な場合はエラー
 */
int	validate_conf(t_config *config, char **av)
{
	if (config->num_coders <= 0)
	{
		fprintf(stderr, "Error: number_of_coders must be strictly positive\n");
		return (0);
	}
	if (strcmp(av[8], "fifo") != 0 && strcmp(av[8], "edf") != 0)
	{
		fprintf(stderr, "Error: scheduler must be 'fifo' or 'edf'\n");
		return (0);
	}
	config->scheduler = av[8];
	return (1);
}
