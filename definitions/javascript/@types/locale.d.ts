declare module "locale" {
  /**
   *
   * @param key 国际化的key
   * @returns 国际化后的值
   *
   * @example
   * en_us.lang: example.title = “test data”
   *
   * i18n('example.title') // "test data"
   */
  const i18n: (key: string) => string;
}
