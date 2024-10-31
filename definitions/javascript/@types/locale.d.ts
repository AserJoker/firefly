declare module "locale" {
  /**
   *
   * @param key 国际化的key
   * @returns 国际化后的值
   *
   * @example
   * i18n('example.title') // "test data"
   */
  const i18n: (key: string) => string;
}
